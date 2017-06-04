/**
  ******************************************************************************
  * @file    GlassBreaking/Src/AudioProcess.c 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   Audio Processing Related Functions
  ******************************************************************************
**/


#include "AudioProcess.h"
#include "AudioConfig.h"
#include "IIRFilters.h"
#include "HardwareConfig.h"
#include "LogisticPrediction.h"

/* Globals */

extern DFSDM_Channel_HandleTypeDef  DfsdmChannelHandle;
extern DFSDM_Filter_HandleTypeDef   DfsdmFilterHandle;
extern DMA_HandleTypeDef 					  hdma_dfsdmReg_FLT[4];
extern UART_HandleTypeDef 					uartHandle;

/* DFSM buffers */
extern int32_t                      WaitBuff[NUM_OF_MICS][WAIT_BUFF_LEN];
extern int32_t                      AnalyzeBuff[ANALYZE_BUFF_LEN];

extern int16_t											HP_filteredWaitBuff[NUM_OF_MICS][WAIT_BUFF_LEN];
extern int16_t											HP_filteredAnalyzeBuff[ANALYZE_BUFF_LEN];

bool																eventFlag = false; 

int16_t                      				FeaturesBuff[NUM_OF_FILTERS][FEATURE_BUFF_LEN];
q15_t																IIR_features[NUM_OF_FILTERS][NUM_OF_FEATURES];

char																str[DEBUG_STRING_LENGTH];

/*******************************************************************************/
void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{	
	
}



/*******************************************************************************/
void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
	static int16_t	blockIndex = 0;	
	static int16_t	featureIndex = 0;
	bool 						prediction = false;
	int8_t					i, correlation;
	
	if (hdfsdm_filter != &DfsdmFilterHandle) return;
	HPFilter_DFSM_Buffer(eventFlag);
	
	if (!eventFlag)
	{		
			bool res1, res2;
			res1 = CheckForEvent(HP_filteredWaitBuff[0], EVENT_THRESH);
#ifndef SIMBA_BOARD 
		  res2 = CheckForEvent(HP_filteredWaitBuff[1], EVENT_THRESH);
#else
			res2 = false;
#endif
			if(res1 || res2)
			{
				q15_t rms[2] = {0};
				
				for (int j = 0; j < NUM_OF_MICS; j++)
				{
					arm_iir_lattice_q15(&AnalyzeFilters[2].filter,(q15_t*)HP_filteredWaitBuff[j], HP_filteredWaitBuff[j], WAIT_BUFF_LEN);

					arm_rms_q15(HP_filteredWaitBuff[j], WAIT_BUFF_LEN, &rms[j]);
				}
#ifndef SIMBA_BOARD
				
				// cross correlation step - currently the estimation process ignores the correlation and RMS values
				correlation = xcorrelation(HP_filteredWaitBuff[0], HP_filteredWaitBuff[1]);
				if (rms[0] > rms[1])
				{
					//sprintf(str, "correlation value: %d, RMS: %d > %d -> source is on left\r\n", correlation, rms[0], rms[1]);
				} else {
					//sprintf(str, "correlation value: %d, RMS: %d < %d -> source is on right\r\n", correlation, rms[0], rms[1]);
				}
				
#else
				correlation = 0;
#endif

				/* Start event */
				Set_LED();
				eventFlag = true;
				Audio_Stream_Pause();
				DFSDM_Start_AnalyzeEvent();
			}				
	}	
	else
	{
			if(true == AnalyzeSamples(HP_filteredAnalyzeBuff,&blockIndex)) //FeaturesBuff is full
			{
				// Calculate RMS for each filter to make new features
				for(i=0 ; i< NUM_OF_FILTERS ; i++)
				{
					arm_rms_q15(FeaturesBuff[i], FEATURE_BUFF_LEN, &IIR_features[i][featureIndex]);
				}
				featureIndex++;
			}
			
			if (NUM_OF_FEATURES == featureIndex) //all features are ready
			{
				/* Decide */	
				prediction = Predict(IIR_features);
				sprintf(str, "%s\r\n\n", prediction? "GLASS!":"NOT GLASS");
				HAL_UART_Transmit(&uartHandle, (uint8_t*)str, strlen(str), TIMEOUT);
				
				/* Clear event */			
				featureIndex 	= 0;
				blockIndex 		= 0;
				eventFlag 		= false;
				Reset_LED();
				Audio_Stream_Pause();	
				DFSDM_Start_WaitForEvent();
			}					
		}
}



/*******************************************************************************/
void	HPFilter_DFSM_Buffer(bool analyzeType)
{
	int 		i, j;
	int 		AudioInVolume = AUDIO_IN_VOL;
	static 	HP_FilterState_TypeDef Analyze_HP_Filter, Wait_HP_Filter;
	
	if(analyzeType)
	{
			for (i = 0; i < ANALYZE_BUFF_LEN; i++)
			{
				Analyze_HP_Filter.Z = ((AnalyzeBuff[i] >> 8) * AudioInVolume) >> 7;
				Analyze_HP_Filter.oldOut = (0xFC * (Analyze_HP_Filter.oldOut + Analyze_HP_Filter.Z - Analyze_HP_Filter.oldIn)) / 256;
				Analyze_HP_Filter.oldIn = Analyze_HP_Filter.Z;
				HP_filteredAnalyzeBuff[i] = (int16_t)SaturaLH(Analyze_HP_Filter.oldOut, -32760, 32760);
			}		
	}	
	else
	{
		for (j = 0; j < NUM_OF_MICS; j++)
			for (i = 0; i < WAIT_BUFF_LEN; i++)
			{			
				Wait_HP_Filter.Z = ((WaitBuff[j][i] >> 8) * AudioInVolume) >> 7;
				Wait_HP_Filter.oldOut = (0xFC * (Wait_HP_Filter.oldOut + Wait_HP_Filter.Z - Wait_HP_Filter.oldIn)) / 256;
				Wait_HP_Filter.oldIn = Wait_HP_Filter.Z;
				HP_filteredWaitBuff[j][i] = (int16_t)SaturaLH(Wait_HP_Filter.oldOut, -32760, 32760);
			}	
	}
}



/*******************************************************************************/
bool CheckForEvent(int16_t buffer[], q15_t	pThresh)
{
	q15_t							rmsResult = 0;
	int16_t						filteredBuff[WAIT_BUFF_LEN] = {0};
	
	arm_iir_lattice_q15(&BP_Thud_filter.filter,(q15_t*)buffer, filteredBuff, WAIT_BUFF_LEN);
	arm_rms_q15(filteredBuff, WAIT_BUFF_LEN, &rmsResult);

	ResetFilter(&BP_Thud_filter, WAIT_BUFF_LEN + FILTERS_ORDER);

	return rmsResult > pThresh;
}



/*******************************************************************************/
bool AnalyzeSamples(int16_t	srcBuffer[], int16_t* blockIndex)
{	
	uint8_t i=0;
	if (0 == *blockIndex) ResetAllAnalyzeFilters();

	for(i = 0; i<NUM_OF_FILTERS ; i++)
	{
		arm_iir_lattice_q15(&AnalyzeFilters[i].filter, (q15_t*)srcBuffer, &FeaturesBuff[i][(*blockIndex)*ANALYZE_BUFF_LEN], ANALYZE_BUFF_LEN);
	}
	(*blockIndex)++;
	
	if(FEATURE_BUFF_MS/ANALYZE_BUFF_LEN_MS == (*blockIndex)) //check if FeaturesBuff is full
	{
		(*blockIndex)=0;
		return true;
	}
	else
	{
		return false;
	}
}


/*******************************************************************************/
int8_t xcorrelation(int16_t x[], int16_t y[])
{
	q15_t t_product[CORR_PRODUCT_LEN] = {0};
	q15_t corr = 0, corr_prev = 0;
	int peak = 0;
	
	/* simple cross correlation calculation */
	for (int i = CORR_MIN; i < CORR_MAX; i++)
	{
		arm_mult_q15(&x[CORR_SIG_CENTER], &y[CORR_SIG_CENTER + i], t_product, CORR_PRODUCT_LEN);
		
		for (int j = 0; j < CORR_SIG_CENTER; j++)
		{
			corr += t_product[j];
		}
		if (corr >= corr_prev)
		{
			peak = i;
		}

		corr_prev = corr;
		corr = 0;
	}
	
	return peak;
}


