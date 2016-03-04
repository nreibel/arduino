#ifndef OS_API_TYPES_H_
#define OS_API_TYPES_H_

#include <stdint.h>

typedef void (*Callback)(void);

typedef enum {
	FALSE = 0,
	TRUE = 1
} boolean;

#define ON  1
#define OFF 0

#define NULL_PTR ((void*)0)

#define READ_PTR(addr, type)        (*((type*)(addr)))
#define READ_PU8(addr)              READ_PTR(addr, uint8_t)

#define WRITE_PTR(addr, val, type)  *((type*)(addr)) = ((type)(val))
#define WRITE_PU8(addr, val)       WRITE_PTR(addr, val, uint8_t)

typedef enum
{
	Status_Not_OK = 0,
	Status_Pending,
	Status_OK
} Std_ReturnType;

#endif /* OS_API_TYPES_H_ */
