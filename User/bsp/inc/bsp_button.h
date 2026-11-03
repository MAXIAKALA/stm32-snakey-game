#ifndef _INCLUDE_BSP_BUTTON_H_
#define _INCLUDE_BSP_BUTTON_H_

#define NO_BUTTON 		0x00
#define BUTTON_START 	0x02
#define BUTTON_NO_CHANGE 0xff

typedef struct {
	unsigned char state;
	unsigned char (*scan)(void);
}BUTTON;

BUTTON *getButtonHandle(void);

#endif /* _INCLUDE_BSP_BUTTON_H_ */
