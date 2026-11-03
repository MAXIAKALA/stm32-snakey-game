#ifndef _DATA_STORE_H_
#define _DATA_STORE_H_

#include <stdbool.h>

#ifndef NULL
	#define NULL 0
#endif /* NULL */

#define OBJECT_HEAD 0x83489254

typedef struct{
	unsigned int size;	
	void *buf;
}Object;

typedef struct{
	unsigned int saveHead;
	unsigned int size;
	void *buf;
}ObjectEx;

/* The flash address must be the block address, otherwise other data may be erased.
   The data stored in the flash must be 32bit aligned */
bool SaveData(unsigned int flashAddress,Object *obj);
bool LoadData(unsigned int flashAddress,Object *obj,unsigned int maxSize);

bool SaveDataEx(unsigned int flashId,unsigned int blockSize,ObjectEx *obj);
bool LoadDataEx(unsigned int flashId,unsigned int blockSize,ObjectEx *obj);

#endif /* _DATA_STORE_H_ */
