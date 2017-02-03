/**
  ******************************************************************************
  * @file    GlassBreaking/Src/main.c 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   Main program body
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"



DFSDM_Channel_HandleTypeDef  DfsdmChannelHandle;
DFSDM_Filter_HandleTypeDef   DfsdmFilterHandle;

DMA_HandleTypeDef 					 hdma_dfsdmReg_FLT[4]; //added

uint16_t                     RecBuff[32];
volatile uint32_t            DmaRecHalfBuffCplt  = 0;
uint32_t                     DmaRecBuffCplt      = 0;
uint32_t                     PlaybackStarted     = 0;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void DFSDM_Init(void);

volatile int16_t x = 0;


/* Private functions ---------------------------------------------------------*/
void aHAL_DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
void aHAL_DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
void LED_GPIO_Init(void);
void aToggle_LED(void);


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  HAL_Init();

  /* Configure the system clock to have a frequency of 80 MHz */
  SystemClock_Config();

  /* Configure LED */
	LED_GPIO_Init();

  /* Initialize DFSDM channels and filter for record */
  DFSDM_Init();

  /* Start DFSDM conversions */
  if(HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&DfsdmFilterHandle, (int32_t*)RecBuff, 32))
  {
    Error_Handler();
  }
  aToggle_LED();
	
	
  /* Start loopback */
  while(1)
  {
		
		x = RecBuff[2];
		x++;
		x++;
		x++;
		
  }
}

/**
	* @breif  We are currently working with system PLL. This is a clock which fed by "Multispeed
						Internal RC Oscillator" (MSI). the RC runs at 40MHz and the system clock runs at 80MHz.
						The prescaler for all CPU busses (AHB, APB) is 1.
						Please refer to section 3.11 in the datasheet
  * @param  None
  * @retval None
  */
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

/**
  * @brief  DFSDM channels and filter initialization
  * @param  None
  * @retval None
  */
static void DFSDM_Init(void)
{
	uint16_t shift_amount = 0;
  uint32_t SincOrder = 0;
  shift_amount = 10;
  SincOrder = DFSDM_FILTER_SINC5_ORDER;
	uint16_t AudioFrequency = 32000;

	/* Configure PLL */
	RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
	RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1N = 43;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1P = 7;
	
  /* Configure PLLSAI prescalers */
  /* Please note that some of these parameters must be consistent with 
  the parameters of the main PLL */
  RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
  RCC_ExCLKInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;   
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1M = 6;  
    
  if(HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct) != HAL_OK)
  {
    Error_Handler();
  }	
	
  /* Initialize channel 2 */
  //__HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&DfsdmChannelHandle);
  DfsdmChannelHandle.Instance                      = DFSDM_Channel2;
  DfsdmChannelHandle.Init.OutputClock.Activation   = ENABLE;
  DfsdmChannelHandle.Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  DfsdmChannelHandle.Init.OutputClock.Divider      = 24; /* 11.294MHz/4 = 2.82MHz */
  DfsdmChannelHandle.Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  DfsdmChannelHandle.Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE; /* N.U. */
  DfsdmChannelHandle.Init.Input.Pins               = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  DfsdmChannelHandle.Init.SerialInterface.Type     = DFSDM_CHANNEL_SPI_RISING;
  DfsdmChannelHandle.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
  DfsdmChannelHandle.Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER; /* N.U. */
  DfsdmChannelHandle.Init.Awd.Oversampling         = 10; /* N.U. */
  DfsdmChannelHandle.Init.Offset                   = 0;
  DfsdmChannelHandle.Init.RightBitShift            = shift_amount;
  
	if(HAL_DFSDM_ChannelGetState(&DfsdmChannelHandle) == HAL_DFSDM_CHANNEL_STATE_RESET)
	{
		/* Init the DFSDM Channel */
		aHAL_DFSDM_ChannelMspInit(&DfsdmChannelHandle);
	}
	else
	{
		Error_Handler();
	}
	
	if(HAL_OK != HAL_DFSDM_ChannelInit(&DfsdmChannelHandle))
  {
    Error_Handler();
  }

  /* Initialize filter 0 */
 //__HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&DfsdmFilterHandle);
  DfsdmFilterHandle.Instance                          = DFSDM_Filter0;
  DfsdmFilterHandle.Init.RegularParam.Trigger         = DFSDM_FILTER_SW_TRIGGER;
  DfsdmFilterHandle.Init.RegularParam.FastMode        = ENABLE;
  DfsdmFilterHandle.Init.RegularParam.DmaMode         = ENABLE;
  DfsdmFilterHandle.Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER; /* N.U. */
  DfsdmFilterHandle.Init.InjectedParam.ScanMode       = DISABLE; /* N.U. */
  DfsdmFilterHandle.Init.InjectedParam.DmaMode        = DISABLE; /* N.U. */
  DfsdmFilterHandle.Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO; /* N.U. */
  DfsdmFilterHandle.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES; /* N.U. */
  DfsdmFilterHandle.Init.FilterParam.SincOrder        = SincOrder;
  DfsdmFilterHandle.Init.FilterParam.Oversampling     = 64; /* 11.294MHz/(4*64) = 44.1KHz */
  DfsdmFilterHandle.Init.FilterParam.IntOversampling  = 1;
  
  if(HAL_DFSDM_FilterGetState(&DfsdmFilterHandle) == HAL_DFSDM_FILTER_STATE_RESET)
  {
    /* Init the DFSDM Filter */
    aHAL_DFSDM_FilterMspInit(&DfsdmFilterHandle);
  }
	else
	{
		Error_Handler();
	}
	
	
	if(HAL_OK != HAL_DFSDM_FilterInit(&DfsdmFilterHandle))
  {
    Error_Handler();
  }

  /* Configure regular channel and continuous mode for filter 0 */
  if(HAL_OK != HAL_DFSDM_FilterConfigRegChannel(&DfsdmFilterHandle, DFSDM_CHANNEL_2, DFSDM_CONTINUOUS_CONV_ON))
  {
    Error_Handler();
  }
}



/**
  * @brief  Board's LED initialization - GPIOA Pin 5
  * @param  None
  * @retval None
  */
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
}



/**
  * @brief  Toggling the board's LED - GPIOA Pin 5
  * @param  None
  * @retval None
  */
void aToggle_LED(void)
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}





/**
  * @brief  This function is executed in case of error occurrence. will never return
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while (1)
  {
    aToggle_LED();
    HAL_Delay(1000);
  }
}



/**
  * @brief  Initializes the DFSDM channel MSP.
  * @param  hdfsdm_channel : DFSDM channel handle.
  * @retval None
  */
void aHAL_DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel)
{
  /* Init of clock, gpio and PLLSAI1 clock */
  GPIO_InitTypeDef GPIO_Init;
  
  /* Enable DFSDM clock */
  __HAL_RCC_DFSDM_CLK_ENABLE();
  
  /* Configure PE9 for DFSDM_CKOUT and PE7 for DFSDM_DATIN2 */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_Init.Pin = GPIO_PIN_2;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Alternate = GPIO_AF6_DFSDM;
	HAL_GPIO_Init(GPIOC, &GPIO_Init);
	
	GPIO_Init.Pin = GPIO_PIN_14;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Alternate = GPIO_AF6_DFSDM;
	HAL_GPIO_Init(GPIOB, &GPIO_Init);
	
	GPIO_Init.Pin = GPIO_PIN_10;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Alternate = GPIO_AF6_DFSDM;
	HAL_GPIO_Init(GPIOB, &GPIO_Init);	

}




/**
  * @brief  Initializes the DFSDM filter MSP.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void aHAL_DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
	
  DMA_HandleTypeDef *hdma_dfsdmReg = NULL;
  
  /* Enable DFSDM clock */
  __HAL_RCC_DFSDM_CLK_ENABLE();
  /* Enable the DMA clock */
  __HAL_RCC_DMA1_CLK_ENABLE();
  
  hdma_dfsdmReg = &hdma_dfsdmReg_FLT[0];
  
  /* Configure the hdma_dfsdmReg handle parameters */
  hdma_dfsdmReg->Init.Request = DMA_REQUEST_0;
  hdma_dfsdmReg->Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_dfsdmReg->Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_dfsdmReg->Init.MemInc = DMA_MINC_ENABLE;
  hdma_dfsdmReg->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_dfsdmReg->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_dfsdmReg->Init.Mode = DMA_CIRCULAR;
  hdma_dfsdmReg->Init.Priority = DMA_PRIORITY_HIGH;

  hdma_dfsdmReg->Instance = DMA1_Channel4;
  
  /* Associate the DMA handle */
  __HAL_LINKDMA(hdfsdm_filter, hdmaReg, *hdma_dfsdmReg);
  
  /* Reset DMA handle state */
  __HAL_DMA_RESET_HANDLE_STATE(hdma_dfsdmReg);
  
  /* Configure the DMA Channel */
  HAL_DMA_Init(hdma_dfsdmReg);
  
  /* DMA IRQ Channel configuration */
  if (hdfsdm_filter->Instance == DFSDM_Filter0) 
  {
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  }	
	
}

/**
  * @brief  SAI MSP Init.
  * @param  hsai : pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{

}

/**
  * @brief  Half regular conversion complete callback. 
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  DmaRecHalfBuffCplt = 1;
}

/**
  * @brief  Regular conversion complete callback. 
  * @note   In interrupt mode, user has to read conversion value in this function
            using HAL_DFSDM_FilterGetRegularValue.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  DmaRecBuffCplt = 1;
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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
