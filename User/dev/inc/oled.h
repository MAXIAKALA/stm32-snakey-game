/*	特别说明，本驱动所有字模均采用逐列式扫描  */

#ifndef _INCLUDE_OLED_H_		
#define _INCLUDE_OLED_H_	

#include "bmp.h"
#include "font.h"
#include "stm32f10x.h"

#define All_LINE 0x08
#define All_COLUMN 0x80

typedef enum{
	Align_Left = 0,		//左对齐
	Align_Center = 1,	//居中对齐
	Align_Right = 2,	//右对齐
}ALIGN;/* 对齐方式 */

typedef struct{
	void (*cls)(void);
	void (*clear)(unsigned int line,unsigned int column);

	void (*showCharacter)(unsigned int x,unsigned int y,char ch,FONT_SIZE size);
	void (*showStringEx)(unsigned int line,char *str,FONT_SIZE size,ALIGN align);
	void (*showString)(unsigned int x,unsigned int y,char *str,FONT_SIZE size,FunctionalState wordWrap);	
	
	void (*showDot)(unsigned int x,unsigned int y);
	void (*showBmp)(unsigned int x,unsigned int y,BMP *bmp);
	void (*showWaveform)(unsigned char*data,unsigned int length);
	void (*showCircle)(unsigned int x,unsigned int y,unsigned int radii);
	
	void (*showLine)(unsigned int s_x,unsigned int s_y,unsigned int e_x,unsigned int e_y);
	void (*showRectangle)(unsigned int x,unsigned int y,unsigned int length,unsigned int width);
	
	void (*memorySynchronous)(void);
}OLED;

/* 对外接口 */
OLED* getOledHandle(void);//获取默认控制结构体

#endif /*_INCLUDE_OLED_H_*/
