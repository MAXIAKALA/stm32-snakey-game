#include "stm32f10x.h"
#include "bsp_button.h"

#define BUTTON_START_PORT GPIOA
#define BUTTON_START_PIN GPIO_Pin_2

static BUTTON _button={0};

/**
 *@function 按钮初始化操作
 *@param 	void
 *@return 	void
 */	
static void bspf_GpioInit(void)
{
	//开启GPIO时钟（根据GPIO微调）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			
	GPIO_InitStructure.GPIO_Pin = BUTTON_START_PIN;
	GPIO_Init(BUTTON_START_PORT, &GPIO_InitStructure);	
}

/**
 *@function 按钮扫描
 *@param 	void
 *@return 	按钮值:unsigned char
 */	
static unsigned char _BSP_buttonScan(void)
{
	unsigned char state=0;
	static unsigned char state_old=0;
		
	if(GPIO_ReadInputDataBit(BUTTON_START_PORT,BUTTON_START_PIN) == Bit_RESET){
		state |= BUTTON_START;
	}		

	if((state&0x7f) != state_old){
		state_old = state&0x7f;
		_button.state = state | 0x80;		
	}else 
		return BUTTON_NO_CHANGE;
	
	return _button.state;
}

/**
 *@function 获取按键控制句柄
 *@param 	void
 *@return 	BUTTON*
 */	
BUTTON *getButtonHandle(void)
{
	static FunctionalState initFlag = DISABLE;
	if(initFlag == ENABLE)
		return &_button;
	
	bspf_GpioInit();
	
	_button.state = 0x00;
	_button.scan = _BSP_buttonScan;
	
	initFlag = ENABLE;
	return &_button;
}
