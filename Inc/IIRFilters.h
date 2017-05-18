/**
  ******************************************************************************
  * @file    GlassBreaking/Inc/IIRFilters.h 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   IIR Filters Related Functions
  ******************************************************************************
**/
#ifndef __IIRFILTERS_H
#define __IIRFILTERS_H

#include "arm_math.h"
#include "ConfigParameters.h"

typedef struct{
	q15_t* 												K;
	q15_t* 												V;
	q15_t* 												pState;
	arm_iir_lattice_instance_q15 	filter;
}LAT_FILTER;

/*
extern LAT_FILTER BP_10K_filter;
extern LAT_FILTER BP_5K_filter;
extern LAT_FILTER BP_300_filter;
extern LAT_FILTER BP_35_filter;
extern LAT_FILTER BP_Thud_filter;
*/

extern LAT_FILTER	AnalyzeFilters[NUM_OF_FILTERS];
extern LAT_FILTER BP_Thud_filter;

void InitFilter(LAT_FILTER* filterStruct, q15_t* K, q15_t* V, q15_t* pState, uint32_t blockSize);
void InitAllFilters(void);
void ResetFilter(LAT_FILTER* filter, uint32_t pStateSize);
void ResetAllAnalyzeFilters(void);

#endif

