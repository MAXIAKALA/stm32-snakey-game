/**
  ******************************************************************************
  * @file    bsp_led.h
  * @author  Jcy(1932857334@qq.com)
  * @version V1.0.0
  * @date    11-March-2025
  * @brief   This file provides all the LED firmware functions.
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

/* Includes ------------------------------------------------------------------*/
#include "bsp_led.h"
#include "stm32f10x.h"

/** @addtogroup Jcy_Peripheral_Driver
 * @{
 */

/** @defgroup LED_BSP_Driver
 *	@brief	LED board support package driver
 * @{
 */

/** @defgroup Private_TypesDefinitions
  * @{
  */




/**
  * @}
  */

/** @defgroup Private_Defines
  * @{
  */
  
#define BINDING_API(A,B) (A=B)

/** @defgroup LED_PeripheralPort
  * @{
  */
  
#define LED_1_PORT	GPIOC
#define LED_1_PIN	GPIO_Pin_13

#define LED_2_PORT	GPIOA
#define LED_2_PIN	GPIO_Pin_3

/**
  * @}
  */




/**
  * @}
  */

/** @defgroup Private_Macros
  * @{
  */





/**
  * @}
  */

/** @defgroup Private_Variables
  * @{
  */
  
static LED __led={0};
static const unsigned char __led_ids[LED_MAX_NUM]
	= {LED_1,LED_2};
static GPIO_TypeDef* const __ports[LED_MAX_NUM]
	= {LED_1_PORT,LED_2_PORT};
static const unsigned short __pins[LED_MAX_NUM]
	= {LED_1_PIN,LED_2_PIN};

/**
  * @}
  */

/** @defgroup Private_FunctionPrototypes
  * @{
  */

static void reset(void);
static void close(void);
static void toggle(unsigned int id);	
static void lighten(unsigned int id);	
static void extinguish(unsigned int id);
static void portInit(GPIO_TypeDef *port,unsigned short pins);

/**
  * @}
  */

/** @defgroup Private_Functions
  * @{
  */

/**
  * @brief  Reset peripheral.
  * @param  None
  * @retval None
  */
static void reset(void)
{
	for(int i=0;i<LED_MAX_NUM;i++){
		portInit(__ports[i],__pins[i]);
	}
}

/**
  * @brief  Close peripheral.
  * @param  None
  * @retval None
  */
static void close(void)
{

}

/**
  * @brief  Toggle LED peripheral.
  * @param[in]  id: @ref LED_ID
  * @retval None
  */
static void toggle(unsigned int id)
{
	for(int i=0;i<LED_MAX_NUM;i++){
		if(__led_ids[i] == (id&__led_ids[i])){
			__ports[i]->ODR ^= __pins[i];
		}
	}
	
	/* update states */
	*(unsigned short*)&__led.led_states ^= id;
}	

/**
  * @brief  Lighten LED peripheral.
  * @param[in]  id: @ref LED_ID
  * @retval None
  */
static void lighten(unsigned int id)
{
	for(int i=0;i<LED_MAX_NUM;i++){
		if(__led_ids[i] == (id&__led_ids[i])){
			#if LED_ACTIVE_LEVEL == LED_LEVEL_LOW_ACTIVE
				__ports[i]->BRR = __pins[i];
			#else
				__ports[i]->SBRR = __pins[i];
			#endif /* LED_ACTIVE_LEVEL */
		}
	}
	
	/* update states */
	*(unsigned short*)&__led.led_states |= id;
}

/**
  * @brief  Extinguish LED peripheral.
  * @param[in]  id: @ref LED_ID
  * @retval None
  */
static void extinguish(unsigned int id)
{
	for(int i=0;i<LED_MAX_NUM;i++){
		if(__led_ids[i] == (id&__led_ids[i])){
			#if LED_ACTIVE_LEVEL == LED_LEVEL_LOW_ACTIVE
				__ports[i]->BSRR = __pins[i];
			#else
				__ports[i]->BRR = __pins[i];
			#endif /* LED_ACTIVE_LEVEL */
		}
	}	

	/* update states */
	*(unsigned short*)&__led.led_states &= (~id);
}

/**
  * @brief  LED GPIO initialization.
  * @param[in]  port: where x can be (A..G) to select the GPIO peripheral.
  *	@param[in]  pins: specifies the port bits to be init.
  *		When selecting more than one pin, they must be on the same port.
  * @retval None
  */

static void portInit(GPIO_TypeDef *port,unsigned short pins)
{
	/* Example Enable port clock. */
	RCC_APB2PeriphClockCmd(0x04 << (((unsigned int)port 
		- ((unsigned int)APB2PERIPH_BASE + 0x0800))/0x400),ENABLE);
	
	/* The working status of the port is set. */
	GPIO_InitTypeDef GpioInitStruct={0};
	GpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		
	/* Example Initialize the port */
	GpioInitStruct.GPIO_Pin = pins;
	GPIO_Init(port,&GpioInitStruct);
}
  
/**
  * @}
  */

/** @defgroup Exported_Functions
  * @{
  */
  
/**
 * @brief  		Obtain the control handle of the peripheral through this interface.
 * @param[in] 	key:If a key is required in a special period, 0 is usually transmitted.
 * @return 		A handle structure for manipulating peripherals.
 * @retval		XXX*	A handle,If the value is NULL, an unexpected error occurs.
 * @see		
 * @note
 * @warning
 */	
LED* getLEDHandle(unsigned int key)
{
	static bool init_flag = false;
	if(true == init_flag){
		return &__led;
	}
	
	reset();

	extinguish(LED_1);
	lighten(LED_2);
	
	BINDING_API(__led.reset,reset);
	BINDING_API(__led.close,close);
	BINDING_API(__led.toggle,toggle);
	BINDING_API(__led.lighten,lighten);
	BINDING_API(__led.extinguish,extinguish);
		
	init_flag = true;
	return &__led;
}
  
  
  
/**
  * @}
  */

/**
  * @}
  */
  
/**
  * @}
  */

/******************* (C) COPYRIGHT 2025 Jcy *****END OF FILE****/
