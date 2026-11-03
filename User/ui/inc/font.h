#ifndef __FONT_H__
#define __FONT_H__

typedef struct{
	unsigned int width;
	unsigned int height;	
	void *buffer;
	unsigned short code;
}FontAdapter;

typedef enum {
	FONT_SIZE_1616 = 0x1125,
	FONT_SIZE_2424 = 0x2031,
	FONT_SIZE_3232 = 0x5112,
	FONT_SIZE_6464 = 0x3982,	
}FONT_SIZE;

FontAdapter* getFontAdatper(FONT_SIZE font_size,unsigned short code);

#endif /* __FONT_H__ */
