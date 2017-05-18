/**
  ******************************************************************************
  * @file    GlassBreaking/Src/HardwareConfig.c 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   General MCU Hardware Configuration
  ******************************************************************************
**/

#include "HardwareConfig.h"


/*******************************************************************************/


void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  /* Enable the LSE Oscilator */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* Enable the CSS interrupt in case LSE signal is corrupted or not present */
  HAL_RCCEx_DisableLSECSS();
  
  /* Enable MSI Oscillator and activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM            = 6;
  RCC_OscInitStruct.PLL.PLLN            = 40;
  RCC_OscInitStruct.PLL.PLLP            = 7;
  RCC_OscInitStruct.PLL.PLLQ            = 4;
  RCC_OscInitStruct.PLL.PLLR            = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Enable MSI Auto-calibration through LSE */
  HAL_RCCEx_EnableMSIPLLMode();
	
	/* Select MSI output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_MSI;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);


  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) ;

}


/*******************************************************************************/

void LED_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
  /* Enable clock for GPIO Port A */
	__HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure the GPIO structure */
  GPIO_InitStructure.Pin   = GPIO_PIN_5;
  GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull  = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	
	/* Init GPIO */
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	Reset_LED();
}


/*******************************************************************************/


void Toggle_LED(void)
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}
void Set_LED(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,GPIO_PIN_SET);
}

void Reset_LED(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,GPIO_PIN_RESET);
}

/*******************************************************************************/

void UART_Init(UART_HandleTypeDef *handleStruct)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	/* Enable clock for UART2 & GPIO Port A */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();
	
	/* Set GPIO Port A Pin 2 as Tx */
	GPIO_InitStruct.Pin       = GPIO_PIN_2;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/* Configure UART2 for 115200 baudrate, 8 bits, 1 stop bit */
	handleStruct->Instance = USART2;
  handleStruct->Init.BaudRate = 115200;
  handleStruct->Init.WordLength = UART_WORDLENGTH_8B;
  handleStruct->Init.StopBits = UART_STOPBITS_1;
  handleStruct->Init.Parity = UART_PARITY_NONE;
  handleStruct->Init.Mode = UART_MODE_TX_RX;
  handleStruct->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  handleStruct->Init.OverSampling = UART_OVERSAMPLING_16;
  handleStruct->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  handleStruct->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if(HAL_UART_DeInit(handleStruct) != HAL_OK)
  {
		Error_Handler();
  } 
	if (HAL_UART_Init(handleStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

