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

The data lenght is 256Byte
Sector  -> 16 Data
Block   -> 256 Data
Chip    -> 131072 Data

No        : 4Byte
Platform  : 32Byte
Account   : 32Byte
Password  : 128Byte

Platfrom(32Byte):Github
Account(32Byte):User@exmple.com
Password(AES - 128Byte):6175dbaffbfa020634fef45d79b3d392986ab04f577bd23551e231fe13b7bc2f986ab04f577bd23551e231fe13b7bc2f986ab04f577bd23551e231fe13b7bc2f


-----------------------------
No        : 4Byte
Platform  : 64Byte
Account   : 64Byte
Password  : 32Byte
Data Lenght -> 196Byte

	*/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "Key.h"
#include "usbd_customhid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t InputHistoryCounter = 0;
char *InputHistory[6] = {"🖐", "格", "斗", "键", "盘", "🖐"};
uint8_t report[15] = {0};
uint8_t old_report[15] = {0};
uint8_t Key_scan_counter = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void DisplayInputHistory(void);
void AddInputHistory(const char *u_char);
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
  MX_USB_DEVICE_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

	OLED_Init();
  OLED_Clear();
  OLED_ShowString(0, 0, "Display has OK!", OLED_6X8);
  OLED_Update();
	Key_Init();
  OLED_ShowString(0, 8, "Key has OK!", OLED_6X8);
  OLED_Update();  
  

  HAL_Delay(3000);
	// usb_printf("Initialize done!\r\n"); 
	// usb_printf("HCLK Freq = %d\r\n", HAL_RCC_GetHCLKFreq());
	// usb_printf("PCLK1 Freq = %d\r\n", HAL_RCC_GetPCLK1Freq());
	// usb_printf("PCLK2 Freq = %d\r\n", HAL_RCC_GetPCLK2Freq());
	// usb_printf("SYSCLK Freq = %d\r\n", HAL_RCC_GetSysClockFreq());  

	HAL_TIM_Base_Start_IT(&htim2); 	

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  OLED_Clear();
  OLED_ShowString(0, 0, InputHistory[0], OLED_8X16);
  OLED_ShowString(16, 0, *(InputHistory + 1), OLED_8X16);
  OLED_ShowString(32, 0, InputHistory[2], OLED_8X16);
  OLED_ShowString(48, 0, InputHistory[3], OLED_8X16);
  OLED_ShowString(64, 0, InputHistory[4], OLED_8X16);
  OLED_ShowString(80, 0, InputHistory[5], OLED_8X16);

  OLED_Update();
  OLED_Clear();

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //
    // USBD_HID_SendReport(&hUsbDeviceFS, KeyboardBuff, sizeof(KeyboardBuff) / sizeof(KeyboardBuff[0]));
    //Left
    if (KeyInputBuff[1].KeyState != KEY_UP) report[2] |= 0x01;
    else report[2] &= (~0x01);
    // Up
    if (KeyInputBuff[5].KeyState != KEY_UP) report[4] |= 0x40;
    else report[4] &= (~0x40);
    // Right
    if (KeyInputBuff[2].KeyState != KEY_UP) report[2] |= 0x08;
    else report[2] &= (~0x08);
    // Down
    if (KeyInputBuff[0].KeyState != KEY_UP) report[4] |= 0x04;
    else report[4] &= (~0x04);

    // LP
    if (KeyInputBuff[12].KeyState != KEY_UP) report[4] |= 0x10;
    else report[4] &= (~0x10);
    // MP
    if (KeyInputBuff[7].KeyState != KEY_UP) report[3] |= 0x01;
    else report[3] &= (~0x01);
    // HP
    if (KeyInputBuff[11].KeyState != KEY_UP) report[3] |= 0x40;
    else report[3] &= (~0x40);
    // LK
    if (KeyInputBuff[13].KeyState != KEY_UP) report[3] |= 0x02;
    else report[3] &= (~0x02);
    // MK
    if (KeyInputBuff[8].KeyState != KEY_UP) report[3] |= 0x04;
    else report[3] &= (~0x04);
    // HK
    if (KeyInputBuff[3].KeyState != KEY_UP) report[3] |= 0x08;
    else report[3] &= (~0x08);

    // LP+LK
    if (KeyInputBuff[10].KeyState != KEY_UP) report[5] |= 0x01;
    else report[5] &= (~0x01);
    // MP+MK
    if (KeyInputBuff[4].KeyState != KEY_UP) report[2] |= 0x80;
    else report[2] &= (~0x80);
    // HP+HK
    if (KeyInputBuff[6].KeyState != KEY_UP) report[3] |= 0x20;
    else report[3] &= (~0x20);
    // LP+MP+HP+LK+MK+HK
    if (KeyInputBuff[9].KeyState != KEY_UP) report[4] |= 0x08;
    else report[4] &= (~0x08);    

    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, sizeof(report));
    DisplayInputHistory();
    
    // HAL_Delay(10);
    OLED_Update();
    OLED_Clear();
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

void DisplayInputHistory(void){
  if((report[2] != old_report[2]) || (report[3] != old_report[3]) || (report[4] != old_report[4]) || (report[5] != old_report[5])){
    old_report[2] = report[2];
    old_report[3] = report[3];
    old_report[4] = report[4];
    old_report[5] = report[5];

    if(report[4] == 0x40 && report[2] == 0x01) AddInputHistory("↖");
    else if(report[4] == 0x40 && report[2] == 0x08) AddInputHistory("↗");
    else if(report[4] == 0x04 && report[2] == 0x01) AddInputHistory("↙");
    else if(report[4] == 0x04 && report[2] == 0x08) AddInputHistory("↘");
    else if(report[2] == 0x01) AddInputHistory("←");
    else if(report[2] == 0x08) AddInputHistory("→");
    else if(report[4] == 0x40) AddInputHistory("↑");
    else if(report[4] == 0x04) AddInputHistory("↓");
    else if((report[4] == 0x10) && (report[3] == 0x01) ||
            (report[4] == 0x10) && (report[3] == 0x40) ||
            (report[3] == 0x40) && (report[3] == 0x01) || 
            (report[3] == 0x02) && (report[3] == 0x04) || 
            (report[3] == 0x02) && (report[3] == 0x08) || 
            (report[3] == 0x04) && (report[3] == 0x08)) AddInputHistory("💨");
    else if(report[4] == 0x10) AddInputHistory("🖐");
    else if(report[3] == 0x01) AddInputHistory("👊");
    else if(report[3] == 0x40) AddInputHistory("💪");
    else if(report[3] == 0x02) AddInputHistory("🦶");
    else if(report[3] == 0x04) AddInputHistory("🦵");
    else if(report[3] == 0x08) AddInputHistory("🦿");
    else if(report[5] == 0x01) AddInputHistory("🫱");
    else if(report[2] == 0x80) AddInputHistory("🔴");
    else if(report[3] == 0x20) AddInputHistory("🛡");
    else if(report[4] == 0x08) AddInputHistory("😜");
    
  }
  // if(report[2] != old_report[2]){
  //   old_report[2] = report[2];
  //   switch (report[2])
  //   {
  //   case 0x01:AddInputHistory("←");break;
  //   case 0x08:AddInputHistory("→");break;
  //   default:
  //     break;
  //   }    
  // }
  if(report[3] != old_report[3]){
    old_report[3] = report[3];
    switch (report[3])
    {
    // case 0x01:AddInputHistory("🖐");break;
    // case 0x02:AddInputHistory("👊");break;
    // case 0x04:AddInputHistory("💪");break;
    // case 0x08:AddInputHistory("🦶");break;
    // case 0x10:AddInputHistory("🦵");break;
    // case 0x20:AddInputHistory("🦿");break;
    case 0x01 | 0x02:
    case 0x01 | 0x04:
    case 0x02 | 0x04:
    case 0x08 | 0x10:
    case 0x08 | 0x20:
    case 0x10 | 0x20:AddInputHistory("💨");break;
    case 0x01 | 0x08:AddInputHistory("🫱");break;
    case 0x02 | 0x10:AddInputHistory("🛡");break;
    case 0x04 | 0x20:AddInputHistory("🔴");break;
    default:
      break;
    }    
  }
  // if(report[4] != old_report[4]){
  //   old_report[4] = report[4];
  //   switch (report[4])
  //   {
  //     case 0x40:AddInputHistory("↑");break;
  //     case 0x04:AddInputHistory("↓");break;
  //     default:
  //       break;
  //   }    
  // }    
  if(report[5] != old_report[5]){
    old_report[5] = report[5];
    switch (report[5])
    {
    case 0x01:AddInputHistory("🖐");break;
    case 0x02:AddInputHistory("👊");break;
    case 0x04:AddInputHistory("💪");break;
    case 0x08:AddInputHistory("🦶");break;
    case 0x10:AddInputHistory("🦵");break;
    case 0x20:AddInputHistory("🦿");break;
    case 0x01 | 0x02:
    case 0x01 | 0x04:
    case 0x02 | 0x04:
    case 0x08 | 0x10:
    case 0x08 | 0x20:
    case 0x10 | 0x20:AddInputHistory("气");break;
    case 0x01 | 0x08:AddInputHistory("🫱");break;
    case 0x02 | 0x10:AddInputHistory("🛡");break;
    case 0x04 | 0x20:AddInputHistory("🔴");break;
    default:
      break;
    }    
  }   
  OLED_ShowString(0, 0, InputHistory[0], OLED_8X16);
  OLED_ShowString(16, 0, *(InputHistory + 1), OLED_8X16);
  OLED_ShowString(32, 0, InputHistory[2], OLED_8X16);
  OLED_ShowString(48, 0, InputHistory[3], OLED_8X16);
  OLED_ShowString(64, 0, InputHistory[4], OLED_8X16);
  OLED_ShowString(80, 0, InputHistory[5], OLED_8X16);
}

void AddInputHistory(const char *u_char){
  InputHistory[0] = InputHistory[1];
  InputHistory[1] = InputHistory[2];
  InputHistory[2] = InputHistory[3];
  InputHistory[3] = InputHistory[4];
  InputHistory[4] = InputHistory[5];
  InputHistory[5] = (char*)u_char;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{
		KeyScan(KeyInputBuff + Key_scan_counter++);
		// KeyScan(KeyInputBuff + 1);
		// KeyScan(KeyInputBuff + 2);
		// KeyScan(KeyInputBuff + 3);
		// KeyScan(KeyInputBuff + 4);
		// KeyScan(KeyInputBuff + 5);
		// KeyScan(KeyInputBuff + 6);
		// KeyScan(KeyInputBuff + 7);
		// KeyScan(KeyInputBuff + 8); 
		// KeyScan(KeyInputBuff + 9);
		// KeyScan(KeyInputBuff + 10);
		// KeyScan(KeyInputBuff + 11);
		// KeyScan(KeyInputBuff + 12);
		// KeyScan(KeyInputBuff + 13);    
    Key_scan_counter %= 14;

	}
  if(htim == &htim3){
    
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
