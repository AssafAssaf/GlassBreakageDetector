/**
  ******************************************************************************
  * @file    GlassBreaking/Src/AudioConfig.c 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   Audio Recording Related Functions
  ******************************************************************************
**/

#include "main.h"
#include "AudioConfig.h"
#include "HardwareConfig.h"

#ifndef SIMBA_BOARD
	#define DFSDM_MAIN_CHANNEL DFSDM_Channel2
#else
	#define DFSDM_MAIN_CHANNEL DFSDM_Channel3
#endif

/* Globals */
DFSDM_Channel_HandleTypeDef  		DfsdmChannelHandle[NUM_OF_MICS];
DFSDM_Filter_HandleTypeDef   		DfsdmFilterHandle[NUM_OF_MICS];
DMA_HandleTypeDef 					 		hdma_dfsdmReg_FLT[NUM_OF_MICS];
extern UART_HandleTypeDef 			uartHandle;

/* DFSM buffers */
int32_t                      		WaitBuff[NUM_OF_MICS][WAIT_BUFF_LEN];
int32_t                      		AnalyzeBuff[ANALYZE_BUFF_LEN];

int16_t											 		HP_filteredWaitBuff[NUM_OF_MICS][WAIT_BUFF_LEN];
int16_t											 		HP_filteredAnalyzeBuff[ANALYZE_BUFF_LEN];


/*******************************************************************************/


void DFSDM_Init(void)
{
	uint16_t shift_amount;
  uint32_t SincOrder;
	
  shift_amount = 10;
  SincOrder = DFSDM_FILTER_SINC5_ORDER;

	/* Configure PLL */
	RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
	RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1N = 43;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1P = 7;
	
  /* Configure PLLSAI prescalers */
  RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
  RCC_ExCLKInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;   
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  RCC_ExCLKInitStruct.PLLSAI1.PLLSAI1M = 6;  
    
  if(HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }	
	
  /* Initialize channel 2 */
  DfsdmChannelHandle[0].Instance                      = DFSDM_MAIN_CHANNEL;
  DfsdmChannelHandle[0].Init.OutputClock.Activation   = ENABLE;
  DfsdmChannelHandle[0].Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  DfsdmChannelHandle[0].Init.OutputClock.Divider      = 24;
  DfsdmChannelHandle[0].Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  DfsdmChannelHandle[0].Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE;
  DfsdmChannelHandle[0].Init.Input.Pins               = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  DfsdmChannelHandle[0].Init.SerialInterface.Type     = DFSDM_CHANNEL_SPI_RISING;
  DfsdmChannelHandle[0].Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
  DfsdmChannelHandle[0].Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER;
  DfsdmChannelHandle[0].Init.Awd.Oversampling         = 10;
  DfsdmChannelHandle[0].Init.Offset                   = 0;
  DfsdmChannelHandle[0].Init.RightBitShift            = shift_amount;
  
	if(HAL_DFSDM_ChannelGetState(&DfsdmChannelHandle[0]) == HAL_DFSDM_CHANNEL_STATE_RESET)
	{
		/* Init the DFSDM Channel */
		HAL_DFSDM_ChannelMspInit(&DfsdmChannelHandle[0]);
	}
	else
	{
		Error_Handler();
	}
	
	if(HAL_OK != HAL_DFSDM_ChannelInit(&DfsdmChannelHandle[0]))
  {
    Error_Handler();
  }

  /* Initialize filter 0 */
  DfsdmFilterHandle[0].Instance                          = DFSDM_Filter0;
  DfsdmFilterHandle[0].Init.RegularParam.Trigger         = DFSDM_FILTER_SW_TRIGGER;
  DfsdmFilterHandle[0].Init.RegularParam.FastMode        = ENABLE;
  DfsdmFilterHandle[0].Init.RegularParam.DmaMode         = ENABLE;
  DfsdmFilterHandle[0].Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
  DfsdmFilterHandle[0].Init.InjectedParam.ScanMode       = DISABLE;
  DfsdmFilterHandle[0].Init.InjectedParam.DmaMode        = DISABLE;
  DfsdmFilterHandle[0].Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO;
  DfsdmFilterHandle[0].Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES;
  DfsdmFilterHandle[0].Init.FilterParam.SincOrder        = SincOrder;
  DfsdmFilterHandle[0].Init.FilterParam.Oversampling     = 64;
  DfsdmFilterHandle[0].Init.FilterParam.IntOversampling  = 1;
  
  if(HAL_DFSDM_FilterGetState(&DfsdmFilterHandle[0]) == HAL_DFSDM_FILTER_STATE_RESET)
  {
    /* Init the DFSDM Filter */
    HAL_DFSDM_FilterMspInit(&DfsdmFilterHandle[0]);
  }
	else
	{
		Error_Handler();
	}
	
	if(HAL_OK != HAL_DFSDM_FilterInit(&DfsdmFilterHandle[0]))
  {
    Error_Handler();
  }

  /* Configure regular channel and continuous mode for filter 0 */
#ifndef SIMBA_BOARD
  if(HAL_OK != HAL_DFSDM_FilterConfigRegChannel(&DfsdmFilterHandle[0], DFSDM_CHANNEL_2, DFSDM_CONTINUOUS_CONV_ON))
#else
	if(HAL_OK != HAL_DFSDM_FilterConfigRegChannel(&DfsdmFilterHandle[0], DFSDM_CHANNEL_3, DFSDM_CONTINUOUS_CONV_ON))
#endif
  {
    Error_Handler();
  }
	
#ifndef SIMBA_BOARD	
	/* Initialize channel 7 */
  DfsdmChannelHandle[1].Instance                      = DFSDM_Channel7;
  DfsdmChannelHandle[1].Init.OutputClock.Activation   = ENABLE;
  DfsdmChannelHandle[1].Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  DfsdmChannelHandle[1].Init.OutputClock.Divider      = 24;
  DfsdmChannelHandle[1].Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  DfsdmChannelHandle[1].Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE;
  DfsdmChannelHandle[1].Init.Input.Pins               = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  DfsdmChannelHandle[1].Init.SerialInterface.Type     = DFSDM_CHANNEL_SPI_RISING;
  DfsdmChannelHandle[1].Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
  DfsdmChannelHandle[1].Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER;
  DfsdmChannelHandle[1].Init.Awd.Oversampling         = 10;
  DfsdmChannelHandle[1].Init.Offset                   = 0;
  DfsdmChannelHandle[1].Init.RightBitShift            = shift_amount;
  
	if(HAL_DFSDM_ChannelGetState(&DfsdmChannelHandle[1]) == HAL_DFSDM_CHANNEL_STATE_RESET)
	{
		/* Init the DFSDM Channel */
		HAL_DFSDM_ChannelMspInit(&DfsdmChannelHandle[1]);
	}
	else
	{
		Error_Handler();
	}
	
	if(HAL_OK != HAL_DFSDM_ChannelInit(&DfsdmChannelHandle[1]))
  {
    Error_Handler();
  }

  /* Initialize filter 2 */
  DfsdmFilterHandle[1].Instance                          = DFSDM_Filter2;
  DfsdmFilterHandle[1].Init.RegularParam.Trigger         = DFSDM_FILTER_SW_TRIGGER;
  DfsdmFilterHandle[1].Init.RegularParam.FastMode        = ENABLE;
  DfsdmFilterHandle[1].Init.RegularParam.DmaMode         = ENABLE;
  DfsdmFilterHandle[1].Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
  DfsdmFilterHandle[1].Init.InjectedParam.ScanMode       = DISABLE;
  DfsdmFilterHandle[1].Init.InjectedParam.DmaMode        = DISABLE;
  DfsdmFilterHandle[1].Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO;
  DfsdmFilterHandle[1].Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES;
  DfsdmFilterHandle[1].Init.FilterParam.SincOrder        = SincOrder;
  DfsdmFilterHandle[1].Init.FilterParam.Oversampling     = 64;
  DfsdmFilterHandle[1].Init.FilterParam.IntOversampling  = 1;
  
  if(HAL_DFSDM_FilterGetState(&DfsdmFilterHandle[1]) == HAL_DFSDM_FILTER_STATE_RESET)
  {
    /* Init the DFSDM Filter */
    HAL_DFSDM_FilterMspInit(&DfsdmFilterHandle[1]);
  }
	else
	{
		Error_Handler();
	}
	
	if(HAL_OK != HAL_DFSDM_FilterInit(&DfsdmFilterHandle[1]))
  {
    Error_Handler();
  }

  /* Configure regular channel and continuous mode for filter 0 */
  if(HAL_OK != HAL_DFSDM_FilterConfigRegChannel(&DfsdmFilterHandle[1], DFSDM_CHANNEL_7, DFSDM_CONTINUOUS_CONV_ON))
  {
    Error_Handler();
  }
#endif

}


/*******************************************************************************/


void HAL_DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel)
{
  GPIO_InitTypeDef GPIO_Init;
  
  /* Enable DFSDM, GPIOB, GPIOC clock */
  __HAL_RCC_DFSDM_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
  /* Configure MIC_CLK GPIO - Port C Pin 2 */
#ifndef SIMBA_BOARD
	GPIO_Init.Pin = GPIO_PIN_2;
#else
	GPIO_Init.Pin = GPIO_PIN_2 | GPIO_PIN_7;
#endif
	GPIO_Init.Mode = GPIO_MODE_AF_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Alternate = GPIO_AF6_DFSDM;
	HAL_GPIO_Init(GPIOC, &GPIO_Init);
	
#ifndef SIMBA_BOARD
	/* Configure MIC_PDM_12 GPIO - Port B Pin 14 */
	GPIO_Init.Pin = GPIO_PIN_14;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Alternate = GPIO_AF6_DFSDM;
	HAL_GPIO_Init(GPIOB, &GPIO_Init);
#endif

	/* Configure MIC_PDM_34 GPIO - Port B Pin 10 */
	GPIO_Init.Pin = GPIO_PIN_10;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Speed = GPIO_SPEED_HIGH;
	GPIO_Init.Alternate = GPIO_AF6_DFSDM;
	HAL_GPIO_Init(GPIOB, &GPIO_Init);


}


/*******************************************************************************/

void HAL_DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
	
  DMA_HandleTypeDef *hdma_dfsdmReg = NULL;
  
  /* Enable DFSDM clock */
  __HAL_RCC_DFSDM_CLK_ENABLE();
  /* Enable the DMA clock */
  __HAL_RCC_DMA1_CLK_ENABLE();
  
	if (hdfsdm_filter->Instance == DFSDM_Filter0)
	{
		hdma_dfsdmReg = &hdma_dfsdmReg_FLT[0];
	}
	
	if (hdfsdm_filter->Instance == DFSDM_Filter2)
	{
		hdma_dfsdmReg = &hdma_dfsdmReg_FLT[1];
	}
  
  /* Configure the hdma_dfsdmReg handle parameters */
  hdma_dfsdmReg->Init.Request = DMA_REQUEST_0;
  hdma_dfsdmReg->Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_dfsdmReg->Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_dfsdmReg->Init.MemInc = DMA_MINC_ENABLE;
  hdma_dfsdmReg->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_dfsdmReg->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_dfsdmReg->Init.Mode = DMA_CIRCULAR;
  hdma_dfsdmReg->Init.Priority = DMA_PRIORITY_HIGH;

	if (hdfsdm_filter->Instance == DFSDM_Filter0)
	{
		hdma_dfsdmReg->Instance = DMA1_Channel4;
	}
	
	if (hdfsdm_filter->Instance == DFSDM_Filter2)
	{
		hdma_dfsdmReg->Instance = DMA1_Channel6;
	}
  
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


/*******************************************************************************/


void HAL_DFSDM_FilterAwdCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter,
                                        uint32_t Channel, uint32_t Threshold)
{
	for(int i=0;i<20;i++)
	{
		Toggle_LED();
		for(int j=0;j<9999999;j++);
	}
}

/*******************************************************************************/

void DFSDM_Start_WaitForEvent(void)
{
  /* Start DFSDM conversions */
  if(HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&DfsdmFilterHandle[0], (int32_t*)WaitBuff[0], WAIT_BUFF_LEN))
  {
    Error_Handler();
  }	
#ifndef SIMBA_BOARD
	if(HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&DfsdmFilterHandle[1], (int32_t*)WaitBuff[1], WAIT_BUFF_LEN))
  {
    Error_Handler();
  }
#endif
}


/*******************************************************************************/
void DFSDM_Start_AnalyzeEvent(void)
{
  /* Start DFSDM conversions */
  if(HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&DfsdmFilterHandle[0], (int32_t*)AnalyzeBuff, ANALYZE_BUFF_LEN))
  {
    Error_Handler();
  }
#ifndef SIMBA_BOARD
	if(HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&DfsdmFilterHandle[1], (int32_t*)AnalyzeBuff, ANALYZE_BUFF_LEN))
  {
    Error_Handler();
  }	
#endif
}


/*******************************************************************************/

uint8_t Audio_Stream_Pause(void)
{    
	if (HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(&DfsdmFilterHandle[0])) 
  {
    return 1;
  }
	if (HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(&DfsdmFilterHandle[1])) 
  {
    return 1;
  }  	
  /* Return 0 if all operations are OK */
  return 0;
}

/*******************************************************************************/

