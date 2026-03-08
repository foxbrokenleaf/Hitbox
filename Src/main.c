/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
__ALIGN_BEGIN static uint8_t HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE]  __ALIGN_END =
{
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x06,        // Usage (Keyboard)
0xA1, 0x01,        // Collection (Application)                                                   
0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
0x19, 0xE0,        //   Usage Minimum (0xE0)
0x29, 0xE7,        //   Usage Maximum (0xE7)
0x15, 0x00,        //   Logical Minimum (0)
0x25, 0x01,        //   Logical Maximum (1)
0x75, 0x01,        //   Report Size (1)
0x95, 0x08,        //   Report Count (8)
0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x95, 0x01,        //   Report Count (1)
0x75, 0x08,        //   Report Size (8)
0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x95, 0x05,        //   Report Count (5)
0x75, 0x01,        //   Report Size (1)
0x05, 0x08,        //   Usage Page (LEDs)
0x19, 0x01,        //   Usage Minimum (Num Lock)
0x29, 0x03,        //   Usage Maximum (Scroll Lock)
0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x95, 0x01,        //   Report Count (1)
0x75, 0x03,        //   Report Size (3)
0x91, 0x01,        //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x95, 0x06,        //   Report Count (6)
0x75, 0x08,        //   Report Size (8)
0x15, 0x00,        //   Logical Minimum (0)
0x25, 0x65,        //   Logical Maximum (101)
0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
0x19, 0x00,        //   Usage Minimum (0x00)
0x29, 0x65,        //   Usage Maximum (0x65)
0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              // End Collection
// 63 bytes
};  

#define HID_EPIN_SIZE                 0x08U	//Keyboard
#define HID_MOUSE_REPORT_DESC_SIZE    63U	//Keyboard

	*/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usbd_hid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum{
	KEY_DOWN = 0U,
	KEY_HOLD,
	KEY_UP
}KeyActionType;
typedef struct{
	GPIO_TypeDef *GPIO_Port;
	uint16_t GPIO_Pin;
	KeyActionType KeyState;
	uint16_t HoldCnt;
}ButtonType;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t KeyInputBuffAddPos = 0;
uint8_t KeyInputBuffExecPos = 0;
ButtonType KeyInputBuff[13] = {
	{.GPIO_Port = MoveLeft_GPIO_Port, .GPIO_Pin = MoveLeft_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = MoveDown_GPIO_Port, .GPIO_Pin = MoveDown_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = MoveRight_GPIO_Port, .GPIO_Pin = MoveRight_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = MoveUp_GPIO_Port, .GPIO_Pin = MoveUp_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = LightPunch_GPIO_Port, .GPIO_Pin = LightPunch_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = MiddlePunch_GPIO_Port, .GPIO_Pin = MiddlePunch_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = HeavyPunch_GPIO_Port, .GPIO_Pin = HeavyPunch_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = LightKick_GPIO_Port, .GPIO_Pin = LightKick_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = MiddleKick_GPIO_Port, .GPIO_Pin = MiddleKick_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = HeavyKick_GPIO_Port, .GPIO_Pin = HeavyKick_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = LPLK_GPIO_Port, .GPIO_Pin = LPLK_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = MPMK_GPIO_Port, .GPIO_Pin = MPMK_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
	{.GPIO_Port = HPHK_GPIO_Port, .GPIO_Pin = HPHK_Pin, .KeyState = KEY_UP, .HoldCnt = 0},
};
uint8_t KeyInputBuffStr[][13] = {
	"MoveLeft",
	"MoveDown",
	"MoveRight",
	"MoveUp",
	"LightPunch",
	"MiddlePunch",
	"HeavyPunch",
	"LightKick",
	"MiddleKick",
	"HeavyKick",
	"LPLK",
	"MPMK",
	"HPHK",
};
/*
buff[0]:
- bit0: Left CTRL
- bit1:	Left SHIFT
- bit2:	Left ALT
- bit3:	Left GUI
- bit4:	Right CTRL
- bit5:	Right SHIFT
- bit6:	Right ALT
- bit7: Right GUI
buff[1]: Padding = Always 0x00
buff[2]: Key 1 
buff[3]: Key 2 
buff[4]: Key 3 
buff[5]: Key 4 
buff[6]: Key 5 
buff[7]: Key 7 
*/
uint8_t KeyboardBuffValuePos = 0;
uint8_t KeyboardBuff[] = {
	0, 0, 0, 0,
	0, 0, 0, 0
};
const uint8_t KeyStreetFighter6[] = {
  4, 22, 7, 44, //ÒÆ¶¯(Move key)
  24, 12, 18, //È­
  13, 14, 15, //½Å
  17, //Í¶
  11, //¶·Æø·ÀÓù
  9, //¶·Æø±Å·Å
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KeyDebug(void);
void AddKeyValue(uint8_t AddKeyValue);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	printf("Initialize done!\r\n"); 
	printf("HCLK Freq = %d\r\n", HAL_RCC_GetHCLKFreq());
	printf("PCLK1 Freq = %d\r\n", HAL_RCC_GetPCLK1Freq());
	printf("PCLK2 Freq = %d\r\n", HAL_RCC_GetPCLK2Freq());
	printf("SYSCLK Freq = %d\r\n", HAL_RCC_GetSysClockFreq());

	HAL_TIM_Base_Start_IT(&htim2); 	//å¼€å®šæ—¶ï¿??

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	// KeyDebug();
	if(KeyInputBuff[0].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[0]);
	if(KeyInputBuff[1].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[1]);
	if(KeyInputBuff[2].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[2]);
	if(KeyInputBuff[3].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[3]);
	if(KeyInputBuff[4].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[4]);
	if(KeyInputBuff[5].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[5]);
	if(KeyInputBuff[6].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[6]);
	if(KeyInputBuff[7].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[7]);
	if(KeyInputBuff[8].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[8]);
	if(KeyInputBuff[9].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[9]);
	if(KeyInputBuff[10].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[10]);
	if(KeyInputBuff[11].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[11]);
	if(KeyInputBuff[12].KeyState != KEY_UP) AddKeyValue(KeyStreetFighter6[12]);
	if(KeyInputBuff[0].KeyState == KEY_UP && 
		KeyInputBuff[1].KeyState == KEY_UP &&
		KeyInputBuff[2].KeyState == KEY_UP &&
		KeyInputBuff[3].KeyState == KEY_UP &&
		KeyInputBuff[4].KeyState == KEY_UP &&
		KeyInputBuff[5].KeyState == KEY_UP &&
		KeyInputBuff[6].KeyState == KEY_UP &&
		KeyInputBuff[7].KeyState == KEY_UP &&
		KeyInputBuff[8].KeyState == KEY_UP &&
		KeyInputBuff[9].KeyState == KEY_UP &&
		KeyInputBuff[10].KeyState == KEY_UP &&
		KeyInputBuff[11].KeyState == KEY_UP &&
		KeyInputBuff[12].KeyState == KEY_UP) AddKeyValue(0x00);


   	USBD_HID_SendReport(&hUsbDeviceFS, KeyboardBuff, sizeof(KeyboardBuff) / sizeof(KeyboardBuff[0]));
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	
	switch(GPIO_Pin){
		case MoveLeft_Pin:
		if(HAL_GPIO_ReadPin(MoveLeft_GPIO_Port, MoveLeft_Pin) == GPIO_PIN_RESET) KeyInputBuff[0].KeyState = KEY_DOWN;
		else KeyInputBuff[0].KeyState = KEY_UP;
    
		__HAL_GPIO_EXTI_CLEAR_IT(MoveLeft_Pin);
		break;
		case MoveDown_Pin:
		if(HAL_GPIO_ReadPin(MoveDown_GPIO_Port, MoveDown_Pin) == GPIO_PIN_RESET) KeyInputBuff[1].KeyState = KEY_DOWN;
		else KeyInputBuff[0].KeyState = KEY_UP;
    
		__HAL_GPIO_EXTI_CLEAR_IT(MoveDown_Pin);
		break;		

	}
	KeyInputBuffAddPos %= 7;
}

void KeyScan(ButtonType *bt){
	if(HAL_GPIO_ReadPin(bt->GPIO_Port, bt->GPIO_Pin) == GPIO_PIN_RESET){
		bt->HoldCnt++;
		if(bt->HoldCnt > 100) bt->KeyState = KEY_HOLD;
	}
	else{
		bt->KeyState = KEY_UP;
		bt->HoldCnt = 0;
	} 
}

void KeysScan(void){

}

void KeyDebug(void){
	uint8_t i = 0;
	uint8_t j = 0;
	for(i = 0;i < 13;i++){
		if(KeyInputBuff[i].KeyState != KEY_UP){
			printf("==========================\r\n");
			printf("Key name: %s\r\n", KeyInputBuffStr[i]);
			printf("Key state: %d(0 = Down 1 = Hold 2 = Up)\r\n", KeyInputBuff[i].KeyState);
			printf("USB data: ");
			for(j = 0;j < 6;j++) printf("%02d ", KeyboardBuff[2 + j]);
			printf("\r\n");	
			printf("==========================\r\n");
		}
	}
}

void AddKeyValue(uint8_t v){
	KeyboardBuff[7] = KeyboardBuff[6];
	KeyboardBuff[6] = KeyboardBuff[5];
	KeyboardBuff[5] = KeyboardBuff[4];
	KeyboardBuff[4] = KeyboardBuff[3];
	KeyboardBuff[3] = KeyboardBuff[2];
	KeyboardBuff[2] = v;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{
		KeyScan(KeyInputBuff);
		KeyScan(KeyInputBuff + 1);
		KeyScan(KeyInputBuff + 2);
		KeyScan(KeyInputBuff + 3);
		KeyScan(KeyInputBuff + 4);
		KeyScan(KeyInputBuff + 5);
		KeyScan(KeyInputBuff + 6);
		KeyScan(KeyInputBuff + 7);
		KeyScan(KeyInputBuff + 8);
		KeyScan(KeyInputBuff + 9);
		KeyScan(KeyInputBuff + 10);
		KeyScan(KeyInputBuff + 11);
		KeyScan(KeyInputBuff + 12);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
