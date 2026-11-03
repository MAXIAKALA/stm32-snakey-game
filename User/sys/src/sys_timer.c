#include "sys_timer.h"

#include <string.h>
#include <stdbool.h>

typedef struct{
	char *name;
	ComRecvState *state;
	void (*callBack)(void);
}ComDevice;

typedef struct{
	unsigned int size;
	const unsigned int maxSize;
	ComDevice device[MAX_COM_DEVICE];
}ComDeviceArray;

static ComDeviceArray _devices={0,MAX_COM_DEVICE,{0}};
static FunctionalState eInitFlag = DISABLE;

//
static void sysTimerInit(unsigned short ms)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1 ;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = ms*1000;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 35;	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3,ENABLE);
}
 
ErrorStatus sysTimer_DeviceExit(char *name)
{
	if(name == NULL){return ERROR;}
	
	for(int i=0;i<_devices.size;i++){
		if(0 == strcmp((const char*)name,(const char*)_devices.device[i].name)){	
			for(int j=i;j<_devices.size;j++)
				memcpy(&_devices.device[j],&_devices.device[j+1],sizeof(ComDevice));
			_devices.size--;
						
			return SUCCESS;
		}		
	}
	
	return ERROR;
}	

ErrorStatus sysTimer_DeviceSignIn(char *name,ComRecvState *state,void (*callBack)(void))
{
	if((name == NULL) || (state == NULL) || (callBack == NULL)){
		return ERROR;	
	}
	
	if(eInitFlag == DISABLE){
		sysTimerInit(SYS_TIMER_CLK_MS);		
		eInitFlag = ENABLE;
	}
	
	if(_devices.size >= _devices.maxSize){
		return ERROR;
	}
		
	for(int i=0;i<_devices.size;i++){
		if(0 == strcmp(name,_devices.device[i].name)){
			return ERROR;
		}		
	}
	
	_devices.device[_devices.size].name = name;
	_devices.device[_devices.size].state = state;
	_devices.device[_devices.size].callBack = callBack;
	_devices.size++;
	
	return SUCCESS;
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	
		for(int i=0;i<_devices.size;i++){
			if(*_devices.device[i].state == ComRecvState_Start)
				*_devices.device[i].state = ComRecvState_Finsh;
			else if(*_devices.device[i].state == ComRecvState_Finsh){
				*_devices.device[i].state = ComRecvState_NULL;
				_devices.device[i].callBack();				
			}
		}
	}
}
