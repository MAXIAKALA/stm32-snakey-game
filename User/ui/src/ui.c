#include "ui.h"
#include "oled.h"
#include "delay.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static UI _ui={0};

/**
 *@function 渲染开机界面
 *@param 	void
 *@return 	void
 */	
static void uiBootAnimation(void)
{
	OLED *oled = getOledHandle();
	
	oled->cls();
	oled->showString(40,4,"贪吃蛇",FONT_SIZE_1616,DISABLE);
	oled->showStringEx(22,"System Init",FONT_SIZE_1616,Align_Center);
	for(int i=0;i<84;i++)
	{
		oled->showLine(i+22,44,i+22,52);
		oled->memorySynchronous();
		//Delay_Ms(10);
	}
}

/**
 *@function 刷新UI页面
 *@param 	void
 *@return 	void
 */	
static void uiPageUpdates(void)
{
	char str[32]={0};
	OLED *oled = getOledHandle();
	
	oled->cls();
	
	switch(_ui.param->page)
	{
		case PAGE_HOME:{
			oled->showRectangle(0,0,128,64);
			oled->showStringEx(20,"START",FONT_SIZE_1616,Align_Center);
			break;
		}		
		
		case PAGE_MAIN:	
		{
			//渲染边框
			oled->showRectangle(0,0,128,64);
			
			//渲染种子			
			oled->showRectangle(_ui.param->snake->seed.x*4,_ui.param->snake->seed.y*4,4,4);
			
			//渲染蛇头
			oled->showRectangle(_ui.param->snake->node[0].x*4,_ui.param->snake->node[0].y*4,4,4);
			
			//渲染蛇身
			for(int length=1;length<_ui.param->snake->length;length++)
			{
					oled->showRectangle(_ui.param->snake->node[length].x*4,_ui.param->snake->node[length].y*4,4,4);
					oled->showRectangle(_ui.param->snake->node[length].x*4+1,_ui.param->snake->node[length].y*4+1,2,2);
			}
			
			break;
		}
		
		case PAGE_RESULT:{
			oled->showRectangle(0,0,128,64);
			
			char msg[32];
			if(_ui.param->snake->die){
				oled->showStringEx(16,"GAME OVER",FONT_SIZE_1616,Align_Center);
				sprintf(msg,"length:%d",_ui.param->snake->length);
				oled->showStringEx(32,msg,FONT_SIZE_1616,Align_Center);
			}else{
				oled->showStringEx(24,"VICTORY",FONT_SIZE_1616,Align_Center);
				//sprintf(msg,"length:%d",_ui.param->snake->length);
				//oled->showStringEx(32,msg,FONT_SIZE_1616,Align_Center);
			}

			break;
		}	
		
		default:return;
	}
	
	oled->memorySynchronous();
}

/**
 *@function 设置UI渲染参数
 *@param 	UiParam *param
 *@return 	void
 */	
static void uiSetUiParam(UiParam *param)
{
	_ui.param = param;
}

/**
 *@function 获取UI控制句柄
 *@param 	void
 *@return 	UI*:struct*
 */	
UI *getUiHandle(void)
{
	static bool initFlag = false;
	if(initFlag == true)return &_ui;
	
	/* 绑定函数句柄 */
	_ui.pageUpdates = uiPageUpdates;
	_ui.bootAnimation = uiBootAnimation;
	_ui.setUiParam = uiSetUiParam;

	initFlag = true;
	return &_ui;
}
