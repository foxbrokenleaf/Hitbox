/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    button.h
  * @brief   按键状态机消抖（按下零延迟） + Custom HID 手柄上报接口。
  ******************************************************************************
  * @attention
  *
  * 上报格式（Report ID = 0x04，共 7 字节）：
  *   GC_Report_Buf[0] = 0x04        固定
  *   GC_Report_Buf[1] = 按键 1~8     （bit0 = 按键 1 ... bit7 = 按键 8）
  *   GC_Report_Buf[2] = 按键 9~16    （bit0 = 按键 9 ... bit7 = 按键 16）
  *   GC_Report_Buf[3] = X 轴（MOVE_Right / MOVE_Left）
  *                      满右 = 0x7F(+127)，满左 = 0x81(-127)，回中 = 0x00
  *   GC_Report_Buf[4] = Y 轴（MOVE_Up / MOVE_Down）
  *                      满上 = 0x81(-127)，满下 = 0x7F(+127)，回中 = 0x00
  *   GC_Report_Buf[5] = Z 轴        固定 0x00
  *   GC_Report_Buf[6] = Rz 轴       固定 0x00
  *
  * 轴值由 SOCD 清洗后的按键位图换算而来，所以"方向键位"和"轴"始终一致：
  *   按住上  -> [1]|=0x08 且 [4]=0x81
  *   按住下  -> [1]|=0x02 且 [4]=0x7F
  *   按住左  -> [1]|=0x04 且 [3]=0x81
  *   按住右  -> [1]|=0x01 且 [3]=0x7F
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_H
#define __BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
/* Custom HID 上报包长度：[0] Report ID + [1][2] 16 个按键 + [3..6] 4 个轴 */
#define GC_REPORT_SIZE 7u

/* Custom HID Report ID（与 CUSTOM_HID_ReportDesc_FS 中的 0x85 0x04 对应） */
#define GC_REPORT_ID 0x04u

/* 按键总数：GC_Report_Buf[1] + GC_Report_Buf[2] 共 16 位 */
#define BUTTON_COUNT 16u

/* 消抖窗口，单位 ms（Button_Scan() 必须按 1ms 周期调用） */
#define BUTTON_DEBOUNCE_MS 5u

/* 按键有效电平（必须与 MX_GPIO_Init() 中的上/下拉配置一致）：
 *   - 按键另一端接 GND（最常见）：引脚内部上拉（GPIO_PULLUP）+ GPIO_PIN_RESET
 *   - 按键另一端接 VCC        ：引脚内部下拉（GPIO_PULLDOWN）+ GPIO_PIN_SET */
#define BUTTON_ACTIVE_LEVEL GPIO_PIN_RESET

/* 方向键位号（必须与 button.c 中 s_buttonPin[] 的 MOVE_* 顺序一致，SOCD 处理使用） */
#define BUTTON_BIT_MOVE_RIGHT 0u
#define BUTTON_BIT_MOVE_DOWN  1u
#define BUTTON_BIT_MOVE_LEFT  2u
#define BUTTON_BIT_MOVE_UP    3u

/* ---- 方向键 -> 模拟轴（GC_Report_Buf[3] = X 轴，[4] = Y 轴）-----------------
 * Report Descriptor 把这 4 个轴声明为 8 位、Logical Min/Max = -127 / +127，
 * 所以满量程必须写 0x81(-127) / 0x7F(+127)，0x00 才是居中。
 * 映射关系：
 *   MOVE_Right -> 满右 0x7F(+127)     MOVE_Left -> 满左 0x81(-127)
 *   MOVE_Up    -> 满上 0x81(-127)     MOVE_Down -> 满下 0x7F(+127)
 * 说明：HID / Windows 手柄规范中 Y 轴正方向朝下，所以"上"是负值。
 * 轴值由 SOCD 清洗后的位图换算，因此轴与按键位永远一致：
 *   上优先模式下按住"上"再按"下"，Y 轴仍然是满上，不会掉到中间或满下。
 * 若在某个软件/游戏里发现左右或上下相反，把对应的 *_INVERT 改成 1 即可（无需改代码）。 */
#define BUTTON_AXIS_MIN     0x81u   /* -127：满左 / 满上 */
#define BUTTON_AXIS_MAX     0x7Fu   /* +127：满右 / 满下 */
#define BUTTON_AXIS_CENTER  0x00u   /*    0：居中（松开） */

#ifndef BUTTON_AXIS_ENABLE
#define BUTTON_AXIS_ENABLE 1u       /* 1 = 方向键输出满量程轴值（默认）；0 = [3][4] 恒为 0x00（旧行为） */
#endif

#ifndef BUTTON_AXIS_X_INVERT
#define BUTTON_AXIS_X_INVERT 1u     /* 0 = 右 -> 0x7F，左 -> 0x81（默认）；1 = 左右反相 */
#endif

#ifndef BUTTON_AXIS_Y_INVERT
#define BUTTON_AXIS_Y_INVERT 1u     /* 0 = 上 -> 0x81，下 -> 0x7F（默认）；1 = 上下反相 */
#endif

/* SOCD（Simultaneous Opposite Cardinal Direction，同时按下相反方向）处理模式：
 *   BUTTON_SOCD_NEUTRAL    : 左右同按 -> 中和（两个方向都上报 0）；上下同按 -> 只上报上
 *   BUTTON_SOCD_LAST_INPUT : 左右同按 -> 后按下的方向生效；上下同按 -> 只上报上
 * 说明：Street Fighter 6 等格斗游戏自身对"左右同按"也会做确定化处理（通常中和为 0），
 *       所以 NEUTRAL 模式与游戏内表现一致；若想得到 hitbox 常见的"后按优先"，改成 LAST_INPUT。
 * 垂直方向固定为"上优先"：按住上再按下，下不上报，保证正在进行的上输入（跳跃）不会被打断。 */
#define BUTTON_SOCD_NEUTRAL    1
#define BUTTON_SOCD_LAST_INPUT 2

#ifndef BUTTON_SOCD_MODE
#define BUTTON_SOCD_MODE BUTTON_SOCD_NEUTRAL
#endif

/* Exported variables --------------------------------------------------------*/
/* GC 手柄上报缓冲区：固定字段 + 16 个按键位 + 4 个轴 */
extern uint8_t GC_Report_Buf[GC_REPORT_SIZE];

/* Exported functions prototypes ---------------------------------------------*/
void Button_Init(void);
void Button_Scan(void);
uint16_t Button_GetBits(void);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H */
