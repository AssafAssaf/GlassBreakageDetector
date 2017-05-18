/**
  ******************************************************************************
  * @file    GlassBreaking/Inc/AudioConfig.h 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   Audio Recording Related Functions
  ******************************************************************************
**/
/**
  *   This file configures the following peripherals
  *=============================================================================
  *-----------------------------------------------------------------------------
  *        Port B GPIO Pin 14	                    | MIC_PDM_12
  *-----------------------------------------------------------------------------
  *        Port B GPIO Pin 10                     | MIC_PDM_34
  *-----------------------------------------------------------------------------
  *        Port C GPIO Pin 2											| MIC_CLK
  *-----------------------------------------------------------------------------
	*        DMA 1 Channel 4										  	| Used for audio samples
  *-----------------------------------------------------------------------------
  *=============================================================================
  ******************************************************************************
**/


#ifndef __AUDIOCONFIG_H
#define __AUDIOCONFIG_H

#include "stm32l4xx_hal.h"
#include "ConfigParameters.h"

/**
  * @brief  DFSDM channels and filter initialization
  * @param  None
  * @retval None
  */
void DFSDM_Init(void);

/**
  * @brief  Initializes the DFSDM channel Pins - clock and pdm lines.
  * @param  hdfsdm_channel : DFSDM channel handle.
  * @retval None
  */
void HAL_DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);

/**
  * @brief  Initializes the DFSDM filter MSP.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);

/**
  * @brief  Start DFSDM Conversion
  * @param  None
  * @retval None
  */
void DFSDM_Start_WaitForEvent(void);
void DFSDM_Start_AnalyzeEvent(void);

/**
* @brief  Pauses the audio file stream.
* @param  None
* @retval 0 in case of success, 1 otherwise 
*/
uint8_t Audio_Stream_Pause(void);

#endif
