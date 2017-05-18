/**
  ******************************************************************************
  * @file    GlassBreaking/Inc/HardwareConfig.h 
  * @author  Assaf & Assaf
  * @version V1.0.0
  * @date    30-January-2017
  * @brief   General MCU Hardware Configuration
  ******************************************************************************
**/
/**
  *   This file configures the following peripherals
  *=============================================================================
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 80 MHz
  *-----------------------------------------------------------------------------
  *        Port A GPIO Pin 2											| UART2 Tx (Alternate)
  *-----------------------------------------------------------------------------
  *        Port A GPIO Pin 5	                    | Output GPIO
	*-----------------------------------------------------------------------------
  *=============================================================================
  ******************************************************************************
**/


#ifndef __HWCONFIG_H
#define __HWCONFIG_H

#include "stm32l4xx_hal.h"
#include "main.h"

/**
	* @breif  We are currently working with system PLL clock. This is a clock which fed by "Multispeed
						Internal RC Oscillator" (MSI). the RC runs at 40MHz and the system clock runs at 80MHz (x2 multipication of RC).
						The clocks prescaler for all CPU busses (AHB, APB) is 1.
						Please refer to section 3.11 in the datasheet
  * @param  None
  * @retval None
  */
void SystemClock_Config(void);


/**
  * @brief  Board's LED initialization - GPIOA Pin 5
  * @param  None
  * @retval None
  */
void LED_GPIO_Init(void);

/**
  * @brief  Toggling the board's LED - GPIOA Pin 5
  * @param  None
  * @retval None
  */
void Toggle_LED(void);

/**
  * @brief  Turn on the board's LED  - GPIOA Pin 5
  * @param  None
  * @retval None
  */
void Set_LED(void);

/**
  * @brief  Turn off the board's LED - GPIOA Pin 5
  * @param  None
  * @retval None
  */
void Reset_LED(void);

/**
  * @brief  Initialization of UART2 Peripheral
  * @param  handleStruct - UART structure pointer, will be needed every transmission
  * @retval None
  */
void UART_Init(UART_HandleTypeDef *handleStruct);



#endif
