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
  *   GC_Report_Buf[3] = 0x00        固定
  *   GC_Report_Buf[4] = 0x00        固定
  *   GC_Report_Buf[5] = 0x00        固定
  *   GC_Report_Buf[6] = 0x00        固定
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

/* Exported variables --------------------------------------------------------*/
/* GC 手柄上报缓冲区：固定字段 + 16 个按键位 */
extern uint8_t GC_Report_Buf[GC_REPORT_SIZE];

/* Exported functions prototypes ---------------------------------------------*/
void Button_Init(void);
void Button_Scan(void);
uint16_t Button_GetBits(void);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H */
