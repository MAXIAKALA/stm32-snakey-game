#ifndef _INCLUDE_SYS_CONFIG_H_
#define _INCLUDE_SYS_CONFIG_H_

#include "log.h"
#include "sys_timer.h"
#include <stdbool.h>

#ifndef NULL
	#define NULL 0
#endif /* NULL */

/* FLASH Physical space£¨The first address of the block£© */
#define FLASH_PHYSICAL_SPACE ((*(unsigned short*)0x1FFFF7E0-1)*0x400+0x08000000)

/* System config information */
typedef struct {
	const unsigned int ISN;	//Initial Sequence Number
	

}SYS_INFOR;

/* º¯Êý½Ó¿Ú */
SYS_INFOR* getSysInfor(void);
bool SaveSysInfor(SYS_INFOR *infor);
bool LoadSysInfor(SYS_INFOR *infor);
void FactoryResetCallback(SYS_INFOR *infor);

#endif /* _INCLUDE_SYS_CONFIG_H_ */
