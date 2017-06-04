/**
  ******************************************************************************
  * @file    GlassBreaking/Inc/AudioProcess.h 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   Audio Processing Related Functions
  ******************************************************************************
**/

#ifndef __AUDIOPROCESS_H
#define __AUDIOPROCESS_H

#include <stdbool.h>
#include "stm32l4xx_hal.h"
#include "arm_math.h"
#include "ConfigParameters.h"

#define SaturaLH(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))

#define CORR_MIN -5
#define CORR_MAX 5
#define CORR_PRODUCT_LEN 100
#define CORR_SIG_CENTER 150


/* Structures */

/**
 * @brief   HP filter state structure definition
 */
typedef struct {
	int32_t Z; 
	int32_t oldOut; 
	int32_t oldIn; 
}HP_FilterState_TypeDef;



/**
  * @brief  Half regular conversion complete callback. 
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);


/**
  * @brief  Regular conversion complete callback. 
  * @note   In interrupt mode, user has to read conversion value in this function
            using HAL_DFSDM_FilterGetRegularValue.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);


/**
  * @brief  Fill Buffer with samples after HPF
  * @param  analyzeType : type of event (search/analyze)
  * @retval None
  */
void HPFilter_DFSM_Buffer(bool analyzeType);


/**
  * @brief  Check if buffer energy higher then threshold 
  * @param  buffer : samples buffer
	* @param  pThresh: threshold
  * @retval 1 if thud detected, 0 otherwise
  */
bool CheckForEvent(int16_t	buffer[], q15_t	pThresh);


/**
  * @brief  Analyze samples in analyze state
  * @param  srcBuffer : incoming samples buffer
	* @param  blockIndex : buffer position in 2 seconds recording
  * @retval event type
  */
bool AnalyzeSamples(int16_t	srcBuffer[],int16_t* blockIndex);


/**
  * @brief  Calculate peak correlation between two waves
  * @param  x,y : waves arrays
  * @retval peak correlation index
  */
int8_t xcorrelation(int16_t x[], int16_t y[]);


#endif
