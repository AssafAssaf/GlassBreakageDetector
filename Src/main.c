/**
  ******************************************************************************
  * @file    GlassBreaking/Src/main.c 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   Main program body
  ******************************************************************************
**/



#include "main.h"
#include "HardwareConfig.h"
#include "IIRFilters.h"
#include "AudioConfig.h"
#include "AudioProcess.h"


UART_HandleTypeDef uartHandle;



/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int main(void)
{
  HAL_Init();
	__HAL_RCC_PWR_CLK_ENABLE();
	
  /* Configure the system clock to have a frequency of 40 MHz */
  SystemClock_Config();

  /* Configure LED */
	LED_GPIO_Init();
	
	/* Initialize IIR filters */
	InitAllFilters();
	
	/* Initialize UART for debugging */
	UART_Init(&uartHandle);
	
  /* Initialize DFSDM channels and filter for record in waiting mode */	
  DFSDM_Init();
	DFSDM_Start_WaitForEvent();
	
	/* Start loopback */
  while(1)
	{
		__WFI();
	}
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
		Toggle_LED();
		HAL_Delay(BLINK_DELAY);
  }
}



