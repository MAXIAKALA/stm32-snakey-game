/**
  ******************************************************************************
  * @file    bsp_led.h
  * @author  Jcy(1932857334@qq.com)
  * @version V1.0.0
  * @date    11-March-2025
  * @brief   This file contains all functional prototypes and libraries of LED firmware.
  * @details It mainly includes basic control methods,including reset,close,  
  *			 lighten and lighten, and so on.Note that all operations of the 
  *			 peripheral are based on a unique handle.
  * @copyright	Copyright (c) 2050
  ******************************************************************************
  * @attention
  *
  * THIS FIRMWARE IS FOR REFERENCE ONLY AND IS INTENDED TO PROVIDE USERS WITH 
  * SAVINGS AND TIME SAVINGS. THEREFORE, THE AUTHOR SHALL NOT BE LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES FOR ANY CLAIMS ARISING FROM THE 
  * RETENTION OF COPYRIGHT IN THE FIRMWARE CONTENTS AND THE USE BY THE CUSTOMER
  * OF CODED INFORMATION RELATED TO HIS OR HER PRODUCT.
  *
  *	@attention 
  *
  * If you encounter any problems in the process of use, you can contact the 
  * author via email (1932857334@qq.com), please indicate your intention, if 
  * your demo or project needs to use this firmware, please be sure to retain 
  * copyright information.
  *
  * @par Modify log:
  * <table>
  * <tr><th>Date         <th>Version  <th>Author    <th>Description
  * <tr><td>11-March-2025<td>1.0      <td>Jcy  	 <td>Create initial version
  * </table>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP__H__
#define __BSP__H__

#ifdef __cplusplus
	extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

/** @addtogroup Jcy_Peripheral_Driver
 * @{
 */

/** @addtogroup LED_BSP_Driver
 * @{
 */

/** @defgroup Exported_types
 * @{
 */
 
/**@struct 	LED
*  @brief 	LED Peripheral handle
*/
typedef struct {
	/* only read property */
	const unsigned short led_states;				/**< led state at now. */
	/* property */
	
	/* method */
	void (*reset)(void);					/**< reset peripheral. */
	void (*close)(void);					/**< close peripheral. */
	void (*toggle)(unsigned int id);		/**< toggle leds @ref LED_ID or LED_COLOR. */
	void (*lighten)(unsigned int id);		/**< lighten leds @ref LED_ID or LED_COLOR. */
	void (*extinguish)(unsigned int id);	/**< extinguish leds @ref LED_ID or LED_COLOR. */
}LED;

/**
  *@}
  */

/** @defgroup Exported_Constants
  * @{
  */

/** @defgroup LED_ID 
  * @{
  */

#define LED_1	0x01
#define LED_2	0x02
#define LED_3	0x04
#define LED_4	0x08
#define LED_5	0x10
#define LED_6	0x20
#define LED_7	0x40
#define LED_8	0x80
#define LED_ALL	((LED_1|LED_2)&0xff)
/**
  *@}
  */

/** @defgroup LED_MAX_NUM
  * @{
  */
  
#define LED_MAX_NUM 2

/**
  *@}
  */

/** @defgroup LED_COLOR
  * @{
  */
  
#define LED_RED	LED_2
#define LED_GREEN LED_1

#define LED_SYS LED_1
#define BUZZER LED_2

/**
  *@}
  */

/** @defgroup LED_ACTIVE_LEVEL
  * @{
  */
#define LED_LEVEL_LOW_ACTIVE 0
#define LED_LEVEL_HIGH_ACTIVE 1

#define LED_ACTIVE_LEVEL LED_LEVEL_LOW_ACTIVE

/**
  *@}
  */

/**
  *@}
  */

/** @defgroup Exported_Macros
  * @{
  */



/**
  * @}
  */
  
/** @defgroup Exported_Functions
 * @brief This Peripheral API.
 * @{
 */

/**
 *@brief  		Obtain the control handle of the peripheral through this interface.
 *@param[in] 	key:If a key is required in a special period, 0 is usually transmitted.
 *@return 		A handle structure for manipulating peripherals.
 *@retval		LED*	A handle,If the value is NULL, an unexpected error occurs.
 *@see		
 *@note
 *@warning
 */	
LED* getLEDHandle(unsigned int key);

/**
  *@}
  */

/**
  *@}
  */
  
/**
  *@}
  */

#ifdef __cplusplus
}
#endif

#endif /*__BSP__H__*/

/******************* (C) COPYRIGHT 2025 Jcy *****END OF FILE****/
