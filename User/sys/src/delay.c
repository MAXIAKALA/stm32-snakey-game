#include "Delay.h"
#include "stm32f10x.h"

typedef enum{
	DELAY_US = 0,	//US DELAY
	DELAY_MS = 1,	//MS DELAY
}DelayType;

/**
 *@function 延时功能初始化
 *@param 	DelayType dType 延时类型
 *@return 	void
 */	
static void _delayInit(DelayType dType)
{
	unsigned int sysCoreClock = SystemCoreClock;
	
	switch(dType)
	{
		case DELAY_US:sysCoreClock /= 1000000;break;
		case DELAY_MS:sysCoreClock /= 1000;break;
		default:return;
	}
	
	SysTick_Config(sysCoreClock);
}

/**
 *@function 关闭延时功能（关闭滴答定时器）
 *@param 	void
 *@return 	void
 */	
static void _delayClose(void)
{
	SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;
}

/**
 *@function us级延时函数 
 *@param 	unsigned int us usTime
 *@return 	void
 */	
void Delay_Us(unsigned int us)
{
	_delayInit(DELAY_US);

	for(;us>0;us--)
		while(!((SysTick->CTRL)&(1<<16)));

	_delayClose();
}

/**
 *@function ms级延时函数 
 *@param 	unsigned int ms msTime
 *@return 	void
 */	
void Delay_Ms(unsigned int ms)
{
	_delayInit(DELAY_MS);
	
	for(;ms>0;ms--)
		while(!((SysTick->CTRL)&(1<<16)));
	
	_delayClose();
}
