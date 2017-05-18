/**
  ******************************************************************************
  * @file    GlassBreaking/Inc/ConfigParameters.h 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   Configuration related parameters used by all modules
  ******************************************************************************
**/
#ifndef __CONFIG_PARARMETERS_H
#define __CONFIG_PARARMETERS_H

#define DEBUG_STRING_LENGTH 								500
#define TIMEOUT															1000

#define AUDIO_IN_VOL												1
#define EVENT_THRESH 												1000

#define FILTERS_ORDER												4
#define	NUM_OF_FILTERS											4

#define AUDIO_FREQ_K 												32
#define AUDIO_FREQ_HZ 											AUDIO_FREQ_K * 1000

#define	WAIT_BUFF_LEN_MS										100				// in milliseconds
#define	ANALYZE_BUFF_LEN_MS									1					// in milliseconds
#define	FEATURE_BUFF_MS											100				// in milliseconds
#define	ANALYZE_EVENT_INTERVAL_MS						2000		  // in milliseconds

#define	WAIT_BUFF_LEN												(WAIT_BUFF_LEN_MS * AUDIO_FREQ_K) 		// in samples
#define	ANALYZE_BUFF_LEN										(ANALYZE_BUFF_LEN_MS * AUDIO_FREQ_K)	// in samples
#define	FEATURE_BUFF_LEN										(FEATURE_BUFF_MS * AUDIO_FREQ_K)			// in samples

#define	NUM_OF_FEATURES											(ANALYZE_EVENT_INTERVAL_MS / FEATURE_BUFF_MS)

#ifndef SIMBA_BOARD
	#define NUM_OF_MICS												2
#else
	#define NUM_OF_MICS												1
#endif


#endif
