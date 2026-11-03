#include <stdbool.h>

#include "stm32f10x.h"
#include "sys_timer.h"
#include "bsp_slider.h"

#if FILTER_EN
	#include "kalman.h"	
#endif
#define X_TEST_GPIO	GPIOA
#define X_TEST_PIN	GPIO_Pin_0

#define Y_TEST_GPIO	GPIOA
#define Y_TEST_PIN	GPIO_Pin_1

#define BINDING_API(A,B) (A=B)

#if FILTER_EN
	static KFP __kfp_cur={0};
	static KFP __kfp_bat={0};	
#endif /*FILTER_EN*/
	
static bool __state_run;
static Slider __slider;
static void __comClockCallBack(void);
static ComRecvState __com_clock = ComRecvState_NULL;

/**
 *@function ADC(Analog to Digital Converter) GPIO initialization
 *@param 	GPIO_TypeDef *spADCGpio		ADC¡ªGPIO
 *  		unsigned short usADCGpioPin ADC-PIN
 *			FunctionalState state		@ENABLE or @DISABLE
 *@return 	void
 */	
static void bspf_ADCGpioInit(GPIO_TypeDef *spADCGpio,unsigned short usADCGpioPin,FunctionalState state)
{
	//1.GPIO RCC enable
	RCC_APB2PeriphClockCmd(0x04 << (((unsigned int)spADCGpio 
		- ((unsigned int)APB2PERIPH_BASE + 0x0800))/0x400),ENABLE);
	
	//2.param config
	GPIO_InitTypeDef GpioInitStruct={0};
	if(ENABLE == state)
	{
		GpioInitStruct.GPIO_Mode = GPIO_Mode_AIN;
	}else{
		GpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	}
	
	//3.initialization GPIO
	GpioInitStruct.GPIO_Pin = usADCGpioPin;
	GPIO_Init(spADCGpio,&GpioInitStruct);
}

/**
 *@function ADC£¨Analog Digital Converter£©initailization
 *@param  	FunctionalState state		@ENABLE or @DISABLE
 *@return 	void
 */	
static void bspf_ADCInit(FunctionalState state)
{
	//1.Open the clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,state);
	if(DISABLE == state){return;}
	
	//3.The ADC peripheral to reset
	ADC_DeInit(ADC2);
	
	//4.Configure the ADC peripheral working mode
	ADC_InitTypeDef ADC_InitStructure={0};
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_Init(ADC2,&ADC_InitStructure);

	//2.ADC frequency divider 72 / 6 = 14MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//5.Enable ADC
	ADC_Cmd(ADC2,ENABLE);
	
	//6.Reset and calibration ADC
	ADC_ResetCalibration(ADC2);
	while(ADC_GetResetCalibrationStatus(ADC2));
	
	ADC_StartCalibration(ADC2);
	while(ADC_GetCalibrationStatus(ADC2));
}

/**
 *@function ADC Convert and return the result
 *@param 	unsigned char ADCChannel	
 *@return 	digital quantity 12bits
 */	
static unsigned short bspf_ADCConvert(unsigned char ADCChannel)
{
	if(false == __state_run){
		return 0;
	}
	
	ADC_RegularChannelConfig(ADC2,ADCChannel,1,ADC_SampleTime_239Cycles5);

	ADC_ClearFlag(ADC2,ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC2,ENABLE);
	
	while(!ADC_GetFlagStatus(ADC2,ADC_FLAG_EOC));
		
	return ADC_GetConversionValue(ADC2);	
}

static void bspf_reset(void)
{
	bspf_ADCInit(ENABLE);
	bspf_ADCGpioInit(Y_TEST_GPIO,Y_TEST_PIN,ENABLE);
	bspf_ADCGpioInit(X_TEST_GPIO,X_TEST_PIN,ENABLE);
	
	sysTimer_DeviceSignIn("BAT",&__com_clock,__comClockCallBack);
	__com_clock = ComRecvState_Start;
	
	__state_run = true;
}

static void bspf_stop(void)
{
	bspf_ADCInit(DISABLE);
	
	bspf_ADCGpioInit(Y_TEST_GPIO,Y_TEST_PIN,DISABLE);
	bspf_ADCGpioInit(X_TEST_GPIO,X_TEST_PIN,DISABLE);
	
	sysTimer_DeviceExit("SLIDER");
	
	__state_run = false;
}

static float bspf_updateXVoltage(void)
{
	*(float*)&__slider.x_voltage = 
	#if FILTER_EN	
		kalmanFilter(&__kfp_bat,
	#endif
		bspf_ADCConvert(ADC_Channel_0)*3.3/4096;
	#if FILTER_EN	
	)
	#endif
	;
	
	return __slider.x_voltage;
}

static float bspf_updateYVoltage(void)
{
	*(float*)&__slider.y_voltage =
	#if FILTER_EN		
		kalmanFilter(&__kfp_cur,
	#endif
	bspf_ADCConvert(ADC_Channel_1)*3.3/4096;
	#if FILTER_EN	
	)
	#endif
	;
	
	return __slider.y_voltage;
}

Slider* getSliderHandle(void)
{	
	static bool initFlag = false;
	if(true == initFlag){
		return &__slider;
	}
	
	bspf_reset();
	#if FILTER_EN
		kalmanFilterInit(&__kfp_cur,0.2,0.01,5.43);
		kalmanFilterInit(&__kfp_bat,0.2,0.01,5.43);
	#endif
	
	BINDING_API(__slider.reset,bspf_reset);
	BINDING_API(__slider.stop,bspf_stop);	
		
	initFlag = true;
	return &__slider;
}

static void __comClockCallBack(void)
{
	static unsigned int timer=0;
	
	switch((++timer)%3)
	{
		case 0x00:
			bspf_updateXVoltage();		
		break;
		case 0x01:
			bspf_updateYVoltage();		
		break;
	}
		
	__com_clock=ComRecvState_Start;
}
