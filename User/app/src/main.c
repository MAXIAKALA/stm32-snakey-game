#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "sys_config.h"
#include "delay.h"

#include "ui.h"
#include "bsp_led.h"
#include "bsp_button.h"
#include "bsp_slider.h"

/* 250 ms输出系统时钟 */
static unsigned int SYS_CLK = 500;

/* 利用通讯辅助定时器获取10ms/次 的回调 */
static ComRecvState sysClock;
static unsigned  int timeCount = 0;//计时变量

/**
 *@function 更新系统时钟
 *@param 	void
 *@return 	void
 */	
static void sysClkCallBack(void){timeCount++;sysClock=ComRecvState_Start;};

/**
 *@function 清除系统时钟标志位
 *@param 	void
 *@return 	void
 */	
static void clearSystemClockFlag(void){timeCount = 0;}

/**
 *@function 获取时钟标志位
 *@param 	void
 *@return 	是否到达指定的时间
 */	
static FlagStatus getSystemClockFlag(void){if(timeCount > (SYS_CLK/SYS_TIMER_CLK_MS))return SET;return RESET;}

/**
 *@function 系统初始化操作,建议将所有的初始化操作放在此处
 *@param 	void
 *@return 	void
 */	
static void System_Init(void)
{
	//设置中断优先级分组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	//获取系统时钟速度
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	JLOG(LOG_INFOR,"System clock is %dHz",clocks.SYSCLK_Frequency);

	//系统作为设备注册
	sysTimer_DeviceSignIn("System",&sysClock,sysClkCallBack);
	JLOG(LOG_INFOR,"System start...");
	sysClock = ComRecvState_Start;	

	while(!getSystemClockFlag());
}

/**
 *@function 主函数入口
 *@param 	void
 *@return 	void
 */	
int main(void)
{
	unsigned int start_delay = 8;
	SYS_INFOR *sys_infor = getSysInfor();
		
	/* 系统初始化 */
	System_Init();
	
	LED *led = getLEDHandle(0);
	BUTTON *button = getButtonHandle();
	Slider *slider = getSliderHandle();
	
	const UI *ui = getUiHandle();
	UiParam ui_param={.page=PAGE_HOME};
	ui->setUiParam(&ui_param);
	ui->bootAnimation();
	ui->pageUpdates();

	button->scan();button->scan();
	
	static SnakeNode snake_nodes[32];
	Snake snake; 
	snake.en = false;

		
	ui_param.y_value = (float*)&slider->y_voltage;
	ui_param.x_value = (float*)&slider->x_voltage;
	ui_param.snake = &snake;
		
	while(1)
	{		
		
		SYS_CLK = 500;
		if(snake.en == true)
		{
			if((button->state&BUTTON_START) == BUTTON_START){
				SYS_CLK = 150;
			}
		}
		
		/* 系统时钟触发判断 */
		if(getSystemClockFlag() == SET)
		{
			//清除标志位
			clearSystemClockFlag();

			led->toggle(LED_SYS);	
		
			if(snake.en)
			{			
				//运动方向处理
				if(slider->x_voltage > 2.8){
					if(snake.dir != RIGHT){
						snake.dir = LEFT;
					}
				}else if(slider->x_voltage < 0.5){	
					if(snake.dir != LEFT){
						snake.dir = RIGHT;
					}
				}else if(slider->y_voltage > 2.8){
					if(snake.dir != DOWN){
						snake.dir = UP;
					}
				}else if(slider->y_voltage < 0.5){
					if(snake.dir != UP){
						snake.dir = DOWN;
					}
				}			
				
				//蛇运动
				for(int i=0;i<snake.length-1;i++)
				{
					snake.node[snake.length-1-i].x = snake.node[snake.length-2-i].x;
					snake.node[snake.length-1-i].y = snake.node[snake.length-2-i].y;
				}
				
				switch(snake.dir)
				{
					case LEFT:{
						snake.node[0].x--;					
						break;
					}
					case RIGHT:{
						snake.node[0].x++;					
						break;
					}
					case UP:{
						snake.node[0].y++;					
						break;
					}
					case DOWN:{
						snake.node[0].y--;					
						break;
					}
				}				
				
				//判断是否触墙
				if((snake.node[0].x==0) || (snake.node[0].y==0) || (snake.node[0].x==snake.max_x) || (snake.node[0].y==snake.max_y)){
						snake.end = true;
						snake.die = true;
						snake.en = false;
						ui_param.page = PAGE_RESULT;
										
						led->extinguish(BUZZER);
						Delay_Ms(200);
						led->lighten(BUZZER);	
				}
							
				//判断是否撞击身体
				for(int i=1;i<snake.length;i++)
				{
					if((snake.node[0].x == snake.node[i].x) && (snake.node[0].y == snake.node[i].y))
					{
						snake.end = true;
						snake.die = true;
						snake.en = false;
						ui_param.page = PAGE_RESULT;
						
						led->extinguish(BUZZER);
						Delay_Ms(200);
						led->lighten(BUZZER);						
					}
				}
				
				//判断是否吃到种子
				if((snake.node[0].x==snake.seed.x) && (snake.node[0].y==snake.seed.y)){
					
					//在末尾添加节点
					//末尾处于垂直运动
					if(snake.node[snake.length-2].x == snake.node[snake.length-1].x){
						snake.node[snake.length].x = snake.node[snake.length-1].x;
						snake.node[snake.length].y = snake.node[snake.length-1].y-(snake.node[snake.length-2].y-snake.node[snake.length-1].y);
					}else{
						snake.node[snake.length].y = snake.node[snake.length-1].y;
						snake.node[snake.length].x = snake.node[snake.length-1].x-(snake.node[snake.length-2].x-snake.node[snake.length-1].x);
					}				
					snake.length++;
					if(snake.length >= 30){
						snake.end = true;
						snake.die = false;
						snake.en = false;
						
						
						led->extinguish(BUZZER);
						Delay_Ms(200);
						led->lighten(BUZZER);	
						Delay_Ms(200);						
						led->extinguish(BUZZER);
						Delay_Ms(200);
						led->lighten(BUZZER);	
					}
										
					//生成新的节点
					srand(TIM3->CNT);
					
					bool valid;
					do{
						snake.seed.x = rand()%(snake.max_x-2)+1;
						snake.seed.y = rand()%(snake.max_y-2)+1;
						
						valid = false;
						for(int i=0;i<snake.length;i++)
						{
							if((snake.seed.x==snake.node[i].x) && (snake.seed.y==snake.node[i].y)){
								valid = true;
							}
						}						
					}while(valid);
				}
			}
		}
			
		ui->pageUpdates();
					
	
		
		//按键扫描处理
		if(BUTTON_NO_CHANGE != button->scan()){			
			if((button->state&BUTTON_START) == BUTTON_START){

				if(snake.en == false){
					//初始化贪吃蛇
					snake.length = 2;
					snake.node = snake_nodes;
					snake.node[0].valid = true;
					snake.node[0].x = 4;
					snake.node[0].y = 3;
					snake.node[1].valid = true;
					snake.node[1].x = 3;
					snake.node[1].y = 3;	
					snake.dir = RIGHT;
					
					snake.seed.x = 5;
					snake.seed.y = 5;
					
					snake.max_x = 128/4;
					snake.max_y = 64/4;
					
					snake.en = true;
					
					ui_param.page = PAGE_MAIN;
					
	
					led->extinguish(BUZZER);
					Delay_Ms(200);
					led->lighten(BUZZER);	
				}
			}		
			ui->pageUpdates();
		}
		
		
		
	}
}
