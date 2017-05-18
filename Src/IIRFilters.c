/**
  ******************************************************************************
  * @file    GlassBreaking/Inc/IIRFilters.c 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   IIR Filters Related Functions
  ******************************************************************************
**/
 #include "IIRFilters.h"

LAT_FILTER	AnalyzeFilters[NUM_OF_FILTERS];
LAT_FILTER 	BP_Thud_filter;

/* IIR filters' coefficients */
const q15_t K_10K[]	={24823,	12358,	32143,	12841};
const q15_t V_10K[]	={277,	-396,	-719,	718,	640};

const q15_t K_5K[]	={24823,	-17942,	32149,	-18638};
const q15_t V_5K[]	={277,	575,	-373,	-840,	271};

const q15_t K_300[]	={31871, -32706, 32762, -32718};
const q15_t V_300[]	={12, 48, 48, 0, 0};
//const q15_t V_300[]={3, 12, 12, 0, 0};

const q15_t K_35[]	={32723,	-32767,	32767,	-32767};
const q15_t V_35[]	={1,	0,	0,	0,	0};

/* IIR filters' pState arrays */
q15_t pState_10K[ANALYZE_BUFF_LEN + FILTERS_ORDER] 	= {0} ;
q15_t pState_5K[ANALYZE_BUFF_LEN + FILTERS_ORDER] 	= {0} ;	
q15_t pState_300[ANALYZE_BUFF_LEN + FILTERS_ORDER] 	= {0} ;
q15_t pState_35[ANALYZE_BUFF_LEN + FILTERS_ORDER] 	= {0} ;	
q15_t pState_Thud[WAIT_BUFF_LEN + FILTERS_ORDER] 		= {0} ;	

void InitFilter(LAT_FILTER* filterStruct, q15_t* K, q15_t* V, q15_t* pState, uint32_t blockSize)
{
	filterStruct->K = K;
	filterStruct->V = V;
	filterStruct->pState = pState;
	arm_iir_lattice_init_q15(&filterStruct->filter, FILTERS_ORDER, filterStruct->K, filterStruct->V,
														filterStruct->pState, blockSize);	
}			
			
void InitAllFilters(void)
{
	/* Analyze event filters*/
	InitFilter(&AnalyzeFilters[0], (q15_t*)K_10K, (q15_t*)V_10K, 	pState_10K, ANALYZE_BUFF_LEN);
	InitFilter(&AnalyzeFilters[1], (q15_t*)K_5K, 	(q15_t*)V_5K, 	pState_5K, 	ANALYZE_BUFF_LEN);
	InitFilter(&AnalyzeFilters[2], (q15_t*)K_300, (q15_t*)V_300, 	pState_300, ANALYZE_BUFF_LEN);
	InitFilter(&AnalyzeFilters[3], (q15_t*)K_35, 	(q15_t*)V_35, 	pState_35, 	ANALYZE_BUFF_LEN);
	
	/* Wait for event filter*/
	InitFilter(&BP_Thud_filter, (q15_t*)K_300, (q15_t*)V_300, pState_Thud, WAIT_BUFF_LEN);
}

void ResetFilter(LAT_FILTER* filterStruct, uint32_t pStateSize)
{
	memset(filterStruct->pState,0,pStateSize*sizeof(q15_t));
}
	
void ResetAllAnalyzeFilters(void)
{
		for(int i=0 ; i<NUM_OF_FILTERS ; i++)
		{
			ResetFilter(&AnalyzeFilters[i], ANALYZE_BUFF_LEN + FILTERS_ORDER);
		}
}
