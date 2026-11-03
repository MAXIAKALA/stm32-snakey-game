#include "data_store.h"
#include "sys_config.h"

#include <string.h>

#define MCU_ID_SPACE	0x1FFFF7E8
#define MCU_ID			*(unsigned int*)MCU_ID_SPACE

/* static memory space */
static SYS_INFOR __sys_infor={0};

/**
 *@function Save system information
 *@param 	SYS_INFOR *infor physical space
 *@return 	@false or @true
 */	
bool SaveSysInfor(SYS_INFOR *infor)
{
	Object obj = {sizeof(SYS_INFOR),infor};
	return SaveData(FLASH_PHYSICAL_SPACE,&obj);
}

/**
 *@function load system information
 *@param 	SYS_INFOR *infor physical space
 *@return 	@false or @true
 */	
bool LoadSysInfor(SYS_INFOR *infor)
{
	Object obj = {sizeof(SYS_INFOR),infor};
	return LoadData(FLASH_PHYSICAL_SPACE,&obj,sizeof(SYS_INFOR));
}

/**
 *@function get system information (Built-in memory structure)
 *@param 	void
 *@return 	the static handle
 */	
SYS_INFOR* getSysInfor(void)
{
	if(MCU_ID != __sys_infor.ISN){
		if(LoadSysInfor(&__sys_infor) == false){
		
			FactoryResetCallback(&__sys_infor);
			
			*(unsigned int*)&__sys_infor.ISN = MCU_ID;
			SaveSysInfor(&__sys_infor);
		}
	}
	
	return &__sys_infor;
}

/**
 *@function Factory Reset Callback
 *@param 	SYS_INFOR *infor
 *@return 	void
 */	
__weak void FactoryResetCallback(SYS_INFOR *infor)
{
	memset(infor,0,sizeof(SYS_INFOR));
}
