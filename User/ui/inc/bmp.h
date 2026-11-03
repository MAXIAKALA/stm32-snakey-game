/* 注意添加位图时，必须与液晶的驱动函数采用同样的取模方式 */

#ifndef _INCLUDE_BMP_H_
#define _INCLUDE_BMP_H_	

#ifndef NULL
	#define NULL 0
#endif /* NULL */

typedef enum{
	OPEN_BMP = 41,
	CLOSE_BMP = 34,
	ADJUST_BMP = 54
}BMP_ID;

typedef struct{
	unsigned int height;
	unsigned int width;
	void *bmp;
}BMP;//位图结构

/* 函数声明 */
BMP* getBmpById(BMP_ID id);//获取位图

#endif
