#ifndef _INCLUDE_UI_H_
#define _INCLUDE_UI_H_

#include <stdbool.h>

#define PAGE_SIZE 1

typedef enum {
	LEFT = 1,
	RIGHT,
	UP,
	DOWN
}Direction;

typedef struct{
	bool valid;
	unsigned char x;
	unsigned char y;
}SnakeNode;

typedef struct {
	unsigned int length;	
	Direction dir;
	SnakeNode *node;	
	SnakeNode seed;
	
	unsigned int max_y;
	unsigned int max_x;
	
	bool en;
	bool die;
	bool end;
}Snake;

typedef enum {	
	PAGE_MAIN=1,
	PAGE_RESULT=2,
	PAGE_HOME=3,
}UI_PAGE;

typedef struct{
	UI_PAGE page;
	
	float *x_value;
	float *y_value;
	
	Snake *snake;
}UiParam;

typedef struct {
	const UiParam *param; 				//渲染参数
	void (*setUiParam)(UiParam *param);	//设置渲染参数
	void (*pageUpdates)(void);			//更新页面接口
	void (*bootAnimation)(void);		//开机引导界面
}UI;

/* 对外接口 */
UI *getUiHandle(void);

#endif /* _INCLUDE_UI_H_ */
