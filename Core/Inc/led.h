/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    led.h
  * @brief   调试/指示 LED（PC13）驱动接口。
  ******************************************************************************
  * @attention
  *
  * Blue Pill 板载 LED 接在 PC13 与 3.3V 之间，低电平点亮；
  * 若外接 LED 为高电平点亮，把 LED_ACTIVE_LEVEL 改为 GPIO_PIN_SET 即可。
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
/* LED 点亮电平：PC13 板载 LED 为低电平点亮 */
#define LED_ACTIVE_LEVEL GPIO_PIN_RESET

/* Exported functions prototypes ---------------------------------------------*/
void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);
void LED_Set(uint8_t on);
void LED_Blink(uint8_t times, uint16_t on_ms, uint16_t off_ms);

#ifdef __cplusplus
}
#endif

#endif /* __LED_H */
