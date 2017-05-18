/**
  ******************************************************************************
  * @file    GlassBreaking/Inc/LogisticPrediction.h
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   Prediction related functions
  ******************************************************************************
**/

#ifndef __LOGISTIC_H
#define __LOGISTIC_H

#include <stdbool.h>
#include "arm_math.h"
#include "ConfigParameters.h"

/**
	* @brief  predict true/false based on Theta and the features matrix 
  * @param  features - features matrix 
	* @param  len - features length
  * @retval true/false prediction
  */
bool Predict(q15_t features[][NUM_OF_FEATURES]);

#endif

