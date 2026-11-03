#include "data_store.h"
#include "stm32f10x_flash.h"

/**
 *@function Load data from flash
 *@param 	unsigned int flashAddress	storage flash space
 *			Object *obj					storage object
 *			unsigned int maxSize		obj physical space size
 *@return 	@false or @true
 */	
bool LoadData(unsigned int flashAddress,Object *obj,unsigned int maxSize)
{
	/* Judge whether it is valid */
	if(obj == NULL)return false;
	
	/* Retrieves the object data size from the target address */
	obj->size = *(unsigned int*)flashAddress;
	
	/* Judge whether it is valid */
	if((obj->size == 0x00) || (obj->size > maxSize)|| (obj->size > 1020))return false;
	
	/* Extract data (32bit/once) */
	for(int i=0;i<obj->size/4;i++)
		*(unsigned int*)((unsigned int)obj->buf+i*4) = 
			*(unsigned int*)(flashAddress+(i+1)*4);
	
	/* succeed */
	return true;
}

/**
 *@function Save data from flash
 *@param 	unsigned int flashAddress	storage flash space
 *			Object *obj					storage object
 *@return 	@false or @true
 */	
bool SaveData(unsigned int flashAddress,Object *obj)
{
	/* Judge whether it is valid */
	if((obj == NULL) || (obj->size > (1024-4)))return false;
	
	/* flash unlock */
	FLASH_Unlock();
	
	/* Erase the corresponding sector */
	if(FLASH_ErasePage(flashAddress/1024*1024) != FLASH_COMPLETE)return false;

	/* 32Bit write object size */
	FLASH_ProgramWord(flashAddress,obj->size);
	
	/* Save data (32bit/once) */
	for(int i=0;i<obj->size/4;i++)
		FLASH_ProgramWord(flashAddress+(i+1)*4,
			*(unsigned int*)((unsigned int)obj->buf+i*4));
	
	/* flash lock */
	FLASH_Lock();
	
	/* succeed */
	return true;
}

bool SaveDataEx(unsigned int flashId,unsigned int blockSize,ObjectEx *obj)
{
	if(obj == NULL)return false;	
	
	return false;
}

bool LoadDataEx(unsigned int flashId,unsigned int blockSize,ObjectEx *obj)
{
	if(obj == NULL)return false;

	return false;
}
