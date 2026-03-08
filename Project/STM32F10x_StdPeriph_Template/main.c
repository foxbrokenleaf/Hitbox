/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.6.0
  * @date    20-September-2021
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2011 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include <stdio.h>


/* STM32大容量产品每页大小2KByte，中、小容量产品每页大小1KByte */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) ||\
defined (STM32F10X_CL) || defined (STM32F10X_XL)
#define FLASH_PAGE_SIZE    ((uint16_t)0x800)//2048
#else
#define FLASH_PAGE_SIZE    ((uint16_t)0x400)//1024
#endif

//写入的起始地址与结束地址
#define WRITE_START_ADDR  ((uint32_t)0x08007C00)
#define WRITE_END_ADDR    ((uint32_t)0x08008000)

#ifdef USE_STM32100B_EVAL
 #include "stm32100b_eval_lcd.h"
#elif defined USE_STM3210B_EVAL
 #include "stm3210b_eval_lcd.h"
#elif defined USE_STM3210E_EVAL
 #include "stm3210e_eval_lcd.h" 
#elif defined USE_STM3210C_EVAL
 #include "stm3210c_eval_lcd.h"
#elif defined USE_STM32100E_EVAL
 #include "stm32100e_eval_lcd.h"
#endif

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifdef USE_STM32100B_EVAL
  #define MESSAGE1   "STM32 MD Value Line " 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "  STM32100B-EVAL    " 
#elif defined (USE_STM3210B_EVAL)
  #define MESSAGE1   "STM32 Medium Density" 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "   STM3210B-EVAL    " 
#elif defined (STM32F10X_XL) && defined (USE_STM3210E_EVAL)
  #define MESSAGE1   "  STM32 XL Density  " 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "   STM3210E-EVAL    "
#elif defined (USE_STM3210E_EVAL)
  #define MESSAGE1   " STM32 High Density " 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "   STM3210E-EVAL    " 
#elif defined (USE_STM3210C_EVAL)
  #define MESSAGE1   " STM32 Connectivity " 
  #define MESSAGE2   " Line Device running" 
  #define MESSAGE3   " on STM3210C-EVAL   "
#elif defined (USE_STM32100E_EVAL)
  #define MESSAGE1   "STM32 HD Value Line " 
  #define MESSAGE2   " Device running on  " 
  #define MESSAGE3   "  STM32100E-EVAL    "   
#endif

/* Private macro -------------------------------------------------------------*/
// 月天数表（非闰年）

/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;


/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     

  /* Initialize LEDs, Key Button, LCD and COM port(USART) available on
     STM3210X-EVAL board ******************************************************/

  /* Private variables ---------------------------------------------------------*/

  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);

  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* 使能接收中断 */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    

  /* Retarget the C library printf function to the USARTx, can be USART1 or USART2
     depending on the EVAL board you are using ********************************/
  printf("\n\r %s", MESSAGE1);
  printf(" %s", MESSAGE2);
  printf(" %s\n\r", MESSAGE3);
  {
    RCC_ClocksTypeDef rcc_tmp;
    RCC_GetClocksFreq(&rcc_tmp);
    printf("ADCCLK freq = %d\r\n", rcc_tmp.ADCCLK_Frequency);
    printf("HCLK freq = %d\r\n", rcc_tmp.HCLK_Frequency);
    printf("PCLK1 freq = %d\r\n", rcc_tmp.PCLK1_Frequency);
    printf("PCLK2 freq = %d\r\n", rcc_tmp.PCLK2_Frequency);    
    printf("SYSCLK freq = %d\r\n", rcc_tmp.SYSCLK_Frequency);
  }
  

  /* Turn on leds available on STM3210X-EVAL **********************************/

  /* Add your application code here
     */

  /* Infinite loop */

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef gpio = {
      .GPIO_Mode = GPIO_Mode_Out_PP,
      .GPIO_Pin = GPIO_Pin_12,
      .GPIO_Speed = GPIO_Speed_2MHz
    };
    GPIO_Init(GPIOB, &gpio);

    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
    
    while(1)
    {

    }
}



/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */



void USART1_IRQHandler(void){
    uint8_t data = 0;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        /* 读取接收到的数据 */
        data = USART_ReceiveData(USART1);
        
        /* 这里可以处理接收到的数据 */
        // RxDat = data;
        // USART_SendData(USART1, data);
        // ... 你的处理代码 ...
        
        /* 清除中断标志 */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
    
    /* 其他中断处理 */
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        /* 发送中断处理 */
        USART_ClearITPendingBit(USART1, USART_IT_TXE);
    }
    
    if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET)
    {
        /* 过载错误处理 */
        USART_ClearITPendingBit(USART1, USART_IT_ORE);
    }
}
