/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MoveRight_Pin GPIO_PIN_0
#define MoveRight_GPIO_Port GPIOB
#define MoveRight_EXTI_IRQn EXTI0_IRQn
#define MoveDown_Pin GPIO_PIN_1
#define MoveDown_GPIO_Port GPIOB
#define MoveDown_EXTI_IRQn EXTI1_IRQn
#define MoveLeft_Pin GPIO_PIN_10
#define MoveLeft_GPIO_Port GPIOB
#define MoveLeft_EXTI_IRQn EXTI15_10_IRQn
#define MoveUp_Pin GPIO_PIN_12
#define MoveUp_GPIO_Port GPIOB
#define MoveUp_EXTI_IRQn EXTI15_10_IRQn
#define MPMK_Pin GPIO_PIN_13
#define MPMK_GPIO_Port GPIOB
#define MPMK_EXTI_IRQn EXTI15_10_IRQn
#define HPHK_Pin GPIO_PIN_11
#define HPHK_GPIO_Port GPIOB
#define HPHK_EXTI_IRQn EXTI15_10_IRQn
#define LightKick_Pin GPIO_PIN_15
#define LightKick_GPIO_Port GPIOB
#define LightKick_EXTI_IRQn EXTI15_10_IRQn
#define LightPunch_Pin GPIO_PIN_3
#define LightPunch_GPIO_Port GPIOB
#define LightPunch_EXTI_IRQn EXTI3_IRQn
#define MiddleKick_Pin GPIO_PIN_4
#define MiddleKick_GPIO_Port GPIOB
#define MiddleKick_EXTI_IRQn EXTI4_IRQn
#define HeavyKick_Pin GPIO_PIN_5
#define HeavyKick_GPIO_Port GPIOB
#define HeavyKick_EXTI_IRQn EXTI9_5_IRQn
#define LPLK_Pin GPIO_PIN_6
#define LPLK_GPIO_Port GPIOB
#define LPLK_EXTI_IRQn EXTI9_5_IRQn
#define MiddlePunch_Pin GPIO_PIN_7
#define MiddlePunch_GPIO_Port GPIOB
#define MiddlePunch_EXTI_IRQn EXTI9_5_IRQn
#define HeavyPunch_Pin GPIO_PIN_8
#define HeavyPunch_GPIO_Port GPIOB
#define HeavyPunch_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
