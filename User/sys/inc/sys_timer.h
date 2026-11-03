#ifndef _INCLUDE_SYS_TIMER_H_
#define _INCLUDE_SYS_TIMER_H_

#include "stm32f10x.h"

/* 调试配置宏，正常使用请注释该宏，在调试模式下将输出更多日志信息，影响系统运行效率 */
//#define SYS_TIME_DEBUG_EN
#define SYS_TIMER_CLK_MS	10

/* 最大辅助设备数量,数量过大影响运行速度 */
#define MAX_COM_DEVICE 0x08

/* 通讯接收中断 */
typedef enum {
		ComRecvState_NULL = 0,	//无接收通讯
		ComRecvState_Start = 1,	//接收通讯开始
		ComRecvState_Finsh = 2,	//接收通讯完成
}ComRecvState;

/* 函数接口 */
ErrorStatus sysTimer_DeviceExit(char *name);
ErrorStatus sysTimer_DeviceSignIn(char *name,ComRecvState *state,void (*callBack)(void));

#endif /* _INCLUDE_SYS_TIMER_H_ */
