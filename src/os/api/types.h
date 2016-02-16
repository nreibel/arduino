#ifndef OS_API_TYPES_H_
#define OS_API_TYPES_H_

#include <stdint.h>

typedef void (*Callback)(void);

typedef enum {
	FALSE = 0,
	TRUE = 1
} boolean;



#define NULL_PTR ((void*)0)

#endif /* OS_API_TYPES_H_ */
