/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    led.c
  * @brief   调试/指示 LED（PC13）驱动。
  ******************************************************************************
  * @attention
  *
  * GPIO 配置由 MX_GPIO_Init() 完成（PC13 推挽输出），本文件只负责电平控制。
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "led.h"

/**
  * @brief  初始化 LED（默认熄灭）
  * @retval None
  */
void LED_Init(void)
{
    LED_Off();
}

/**
  * @brief  点亮 LED
  * @retval None
  */
void LED_On(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, LED_ACTIVE_LEVEL);
}

/**
  * @brief  熄灭 LED
  * @retval None
  */
void LED_Off(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,
                      (LED_ACTIVE_LEVEL == GPIO_PIN_RESET) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
  * @brief  翻转 LED 状态
  * @retval None
  */
void LED_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

/**
  * @brief  设置 LED 状态
  * @param  on: 1 = 点亮，0 = 熄灭
  * @retval None
  */
void LED_Set(uint8_t on)
{
    if (on != 0u)
    {
        LED_On();
    }
    else
    {
        LED_Off();
    }
}

/**
  * @brief  LED 闪烁（阻塞），可用于上电自检
  * @param  times: 闪烁次数
  * @param  on_ms: 点亮时长（ms）
  * @param  off_ms: 熄灭时长（ms）
  * @retval None
  */
void LED_Blink(uint8_t times, uint16_t on_ms, uint16_t off_ms)
{
    uint8_t i;

    for (i = 0u; i < times; i++)
    {
        LED_On();
        HAL_Delay(on_ms);
        LED_Off();
        HAL_Delay(off_ms);
    }
}
