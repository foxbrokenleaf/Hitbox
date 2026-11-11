/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    button.c
  * @brief   16 路按键状态机消抖 + Custom HID 上报（按下零延迟触发）。
  ******************************************************************************
  * @attention
  *
  * 单路按键状态机（Button_Scan() 每 1ms 驱动一次）：
  *
  *   RELEASED -------- 原始电平按下 --------> PRESS_DEBOUNCE   （立即置位并上报，零延迟）
  *   PRESS_DEBOUNCE -- 连续按下 >= 5ms -----> PRESSED
  *   PRESS_DEBOUNCE -- 原始电平释放 --------> RELEASE_DEBOUNCE （视为抖动，保持按下）
  *   PRESSED --------- 原始电平释放 --------> RELEASE_DEBOUNCE
  *   RELEASE_DEBOUNCE - 连续释放 >= 5ms ----> RELEASED          （清位并上报）
  *   RELEASE_DEBOUNCE - 原始电平按下 -------> PRESSED           （视为抖动，保持按下）
  *
  * 也就是说：按下沿不做任何延时，状态机只在消抖窗口内确认或回滚，
  * 抖动不会产生多余的按下/释放报文；只有位图变化时才真正发送 USB 报文。
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "button.h"
#include "led.h"
#include "usbd_custom_hid_if.h"

/* Private typedef -----------------------------------------------------------*/
/* 按键状态机状态 */
typedef enum
{
    BTN_STATE_RELEASED = 0,    /* 已释放且稳定 */
    BTN_STATE_PRESS_DEBOUNCE,  /* 刚检测到按下，正在确认（已上报按下） */
    BTN_STATE_PRESSED,         /* 按下已确认 */
    BTN_STATE_RELEASE_DEBOUNCE /* 检测到释放，正在确认（仍上报按下） */
} Button_StateTypeDef;

/* 按键位号到 MCU 引脚的映射 */
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
} Button_PinTypeDef;

/* Private variables ---------------------------------------------------------*/
/* GC 手柄上报缓冲区（固定字段在 Button_ReportTask() 中打包） */
uint8_t GC_Report_Buf[GC_REPORT_SIZE] = {0x00};

/* USB 设备句柄，定义于 usb_device.c */
extern USBD_HandleTypeDef hUsbDeviceFS;

/**
  * @brief 按键位号 -> 引脚映射表，下标即位号（bit0 ~ bit15）
  * @note  需要调整按键顺序时只改这张表即可；
  *        port 为 NULL 的位固定上报 0（本板目前只接了 13 个按键）。
  */
static const Button_PinTypeDef s_buttonPin[BUTTON_COUNT] = {
    {GPIOB, MOVE_Right_Pin}, /* bit0 : MOVE_Right (PB0)  */
    {GPIOB, MOVE_Down_Pin},  /* bit1 : MOVE_Down  (PB1)  */
    {GPIOB, MOVE_Left_Pin},  /* bit2 : MOVE_Left  (PB10) */
    {GPIOB, MOVE_Up_Pin},    /* bit3 : MOVE_Up    (PB12) */
    {GPIOB, BT_A_Pin},       /* bit4 : BT_A       (PB13) */
    {GPIOB, BT_B_Pin},       /* bit5 : BT_B       (PB14) */
    {GPIOB, BT_C_Pin},       /* bit6 : BT_C       (PB15) */
    {GPIOB, BT_D_Pin},       /* bit7 : BT_D       (PB3)  */
    {GPIOB, BT_E_Pin},       /* bit8 : BT_E       (PB4)  */
    {GPIOB, BT_F_Pin},       /* bit9 : BT_F       (PB5)  */
    {GPIOB, BT_G_Pin},       /* bit10: BT_G       (PB6)  */
    {GPIOB, BT_H_Pin},       /* bit11: BT_H       (PB7)  */
    {GPIOB, BT_I_Pin},       /* bit12: BT_I       (PB8)  */
    {NULL, 0u},              /* bit13: 未使用           */
    {NULL, 0u},              /* bit14: 未使用           */
    {NULL, 0u}               /* bit15: 未使用           */
};

/* 每路按键的状态机状态与计数（单位 ms） */
static Button_StateTypeDef s_buttonState[BUTTON_COUNT];
static uint8_t s_buttonCount[BUTTON_COUNT];

/* 当前按键位图 / 最近一次成功上报的位图 */
static uint16_t s_buttonBits = 0u;
static uint16_t s_sentBits = 0u;
static uint8_t s_sentValid = 1u;

/* SOCD 后按优先模式：记录水平方向最近一次被按下的方向 */
#define BUTTON_DIR_NONE  0u
#define BUTTON_DIR_RIGHT 1u
#define BUTTON_DIR_LEFT  2u

#if (BUTTON_SOCD_MODE == BUTTON_SOCD_LAST_INPUT)
static uint8_t s_socdLastHorz = BUTTON_DIR_NONE;
#endif

/* Private function prototypes -----------------------------------------------*/
static uint8_t Button_ReadRaw(uint8_t index);
static void Button_UpdateState(uint8_t index, uint8_t rawActive);
static uint16_t Button_ApplySocd(uint16_t rawBits);
static void Button_ReportTask(void);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  初始化按键状态机与上报缓冲区，需在 GPIO 与 USB 初始化之后调用
  * @retval None
  */
void Button_Init(void)
{
    uint8_t i;

    for (i = 0u; i < BUTTON_COUNT; i++)
    {
        s_buttonState[i] = BTN_STATE_RELEASED;
        s_buttonCount[i] = 0u;
    }

    s_buttonBits = 0u;
    s_sentBits = 0u;
    s_sentValid = 0u;   /* 强制上电后发送一次"全释放"状态，避免主机沿用旧状态 */

    /* 固定字段 */
    GC_Report_Buf[0] = GC_REPORT_ID;
    GC_Report_Buf[1] = 0x00;
    GC_Report_Buf[2] = 0x00;
    GC_Report_Buf[3] = 0x00;
    GC_Report_Buf[4] = 0x00;
    GC_Report_Buf[5] = 0x00;
    GC_Report_Buf[6] = 0x00;
}

/**
  * @brief  按键扫描，必须每 1ms 调用一次（主循环延时调度或 1ms 定时器）
  * @retval None
  */
void Button_Scan(void)
{
    uint16_t bits = 0u;
    uint8_t i;

    for (i = 0u; i < BUTTON_COUNT; i++)
    {
        if (s_buttonPin[i].port == NULL)
        {
            continue; /* 未映射的位恒为 0 */
        }

        Button_UpdateState(i, Button_ReadRaw(i));

        if (s_buttonState[i] != BTN_STATE_RELEASED)
        {
            bits |= (uint16_t)(1u << i);
        }
    }

    /* SOCD 清洗：同时按下相反方向时，按 BUTTON_SOCD_MODE 规则只保留一个方向 */
    bits = Button_ApplySocd(bits);

    if (bits != s_buttonBits)
    {
        s_buttonBits = bits;

        /* 调试指示：任意按键按下（消抖后）点亮 PC13 LED，全部松开熄灭 */
        LED_Set((bits != 0u) ? 1u : 0u);
    }

    /* 位图有变化才打包发送，避免占用 USB 带宽 */
    Button_ReportTask();
}

/**
  * @brief  获取当前已上报的按键位图（bit0 ~ bit15）
  * @retval 按键位图
  */
uint16_t Button_GetBits(void)
{
    return s_buttonBits;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  读取按键原始电平并转换为逻辑值
  * @param  index: 按键位号
  * @retval 1 = 按下，0 = 释放
  */
static uint8_t Button_ReadRaw(uint8_t index)
{
    if (HAL_GPIO_ReadPin(s_buttonPin[index].port, s_buttonPin[index].pin) == BUTTON_ACTIVE_LEVEL)
    {
        return 1u;
    }

    return 0u;
}

/**
  * @brief  单路按键状态机，每 1ms 调用一次
  * @param  index: 按键位号
  * @param  rawActive: 当前原始电平逻辑值（1 = 按下）
  * @retval None
  */
static void Button_UpdateState(uint8_t index, uint8_t rawActive)
{
    switch (s_buttonState[index])
    {
        case BTN_STATE_RELEASED:
            if (rawActive != 0u)
            {
                /* 按下立即生效：本次扫描结束就会打包上报（零延迟） */
                s_buttonState[index] = BTN_STATE_PRESS_DEBOUNCE;
                s_buttonCount[index] = 1u;
            }
            break;

        case BTN_STATE_PRESS_DEBOUNCE:
            if (rawActive != 0u)
            {
                if (s_buttonCount[index] < BUTTON_DEBOUNCE_MS)
                {
                    s_buttonCount[index]++;
                }

                if (s_buttonCount[index] >= BUTTON_DEBOUNCE_MS)
                {
                    /* 按下确认成功 */
                    s_buttonState[index] = BTN_STATE_PRESSED;
                    s_buttonCount[index] = 0u;
                }
            }
            else
            {
                /* 消抖窗口内出现释放：先保持按下进入释放确认，抖动不会产生报文 */
                s_buttonState[index] = BTN_STATE_RELEASE_DEBOUNCE;
                s_buttonCount[index] = 1u;
            }
            break;

        case BTN_STATE_PRESSED:
            if (rawActive == 0u)
            {
                s_buttonState[index] = BTN_STATE_RELEASE_DEBOUNCE;
                s_buttonCount[index] = 1u;
            }
            break;

        case BTN_STATE_RELEASE_DEBOUNCE:
        default:
            if (rawActive != 0u)
            {
                /* 释放窗口内又检测到按下：抖动，恢复按下状态 */
                s_buttonState[index] = BTN_STATE_PRESSED;
                s_buttonCount[index] = 0u;
            }
            else
            {
                if (s_buttonCount[index] < BUTTON_DEBOUNCE_MS)
                {
                    s_buttonCount[index]++;
                }

                if (s_buttonCount[index] >= BUTTON_DEBOUNCE_MS)
                {
                    /* 释放确认成功 */
                    s_buttonState[index] = BTN_STATE_RELEASED;
                    s_buttonCount[index] = 0u;
                }
            }
            break;
    }
}

/**
  * @brief  SOCD 清洗：同时按下相反方向时的确定化处理
  * @param  rawBits: 消抖后的原始按键位图
  * @retval 清洗后的按键位图（将被打包上报）
  */
static uint16_t Button_ApplySocd(uint16_t rawBits)
{
    uint16_t out = rawBits;
    uint8_t right = ((rawBits & (1u << BUTTON_BIT_MOVE_RIGHT)) != 0u) ? 1u : 0u;
    uint8_t left  = ((rawBits & (1u << BUTTON_BIT_MOVE_LEFT))  != 0u) ? 1u : 0u;
    uint8_t up    = ((rawBits & (1u << BUTTON_BIT_MOVE_UP))    != 0u) ? 1u : 0u;
    uint8_t down  = ((rawBits & (1u << BUTTON_BIT_MOVE_DOWN))  != 0u) ? 1u : 0u;

#if (BUTTON_SOCD_MODE == BUTTON_SOCD_LAST_INPUT)
    /* 记录"最近一次新按下的水平方向"（两侧同时按住时保持上一次记录） */
    if ((right != 0u) && (left == 0u))
    {
        s_socdLastHorz = BUTTON_DIR_RIGHT;
    }
    else if ((left != 0u) && (right == 0u))
    {
        s_socdLastHorz = BUTTON_DIR_LEFT;
    }
#endif

    /* 水平方向：左右同时按下 */
    if ((right != 0u) && (left != 0u))
    {
#if (BUTTON_SOCD_MODE == BUTTON_SOCD_LAST_INPUT)
        if (s_socdLastHorz == BUTTON_DIR_RIGHT)
        {
            out &= (uint16_t)~(1u << BUTTON_BIT_MOVE_LEFT);
        }
        else if (s_socdLastHorz == BUTTON_DIR_LEFT)
        {
            out &= (uint16_t)~(1u << BUTTON_BIT_MOVE_RIGHT);
        }
        else
        {
            /* 同一扫描周期同时按下：无法判定先后，按中和处理 */
            out &= (uint16_t)~((1u << BUTTON_BIT_MOVE_LEFT) | (1u << BUTTON_BIT_MOVE_RIGHT));
        }
#else
        /* NEUTRAL：左右同按 -> 中和 */
        out &= (uint16_t)~((1u << BUTTON_BIT_MOVE_LEFT) | (1u << BUTTON_BIT_MOVE_RIGHT));
#endif
    }

    /* 垂直方向：上下同时按下 -> 上优先（CPT 规则，避免出现蹲防+跳的异常） */
    if ((up != 0u) && (down != 0u))
    {
        out &= (uint16_t)~(1u << BUTTON_BIT_MOVE_DOWN);
    }

    return out;
}

/**
  * @brief  打包 GC_Report_Buf 并在按键状态变化时通过 USB 发送
  * @retval None
  */
static void Button_ReportTask(void)
{
    if ((s_sentValid != 0u) && (s_buttonBits == s_sentBits))
    {
        return; /* 按键状态没有变化 */
    }

    if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED)
    {
        return; /* USB 尚未枚举完成，保持待发送状态，下一个扫描周期重试 */
    }

    /* 打包上报：固定字段 + 16 个按键 */
    GC_Report_Buf[0] = GC_REPORT_ID;                           /* 固定 0x04 */
    GC_Report_Buf[1] = (uint8_t)(s_buttonBits & 0xFFu);        /* 按键 1~8  */
    GC_Report_Buf[2] = (uint8_t)((s_buttonBits >> 8) & 0xFFu); /* 按键 9~16 */
    GC_Report_Buf[3] = 0x00;
    GC_Report_Buf[4] = 0x00;
    GC_Report_Buf[5] = 0x00;
    GC_Report_Buf[6] = 0x00;

    if (USBD_CUSTOM_HID_SendReport_FS(GC_Report_Buf, GC_REPORT_SIZE) == USBD_OK)
    {
        s_sentBits = s_buttonBits;
        s_sentValid = 1u;
    }
    /* 返回 USBD_BUSY 说明上一包还没发完：保持待发送状态，下一毫秒自动重发 */
}
