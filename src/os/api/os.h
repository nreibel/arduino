#ifndef OS_API_OS_H_
#define OS_API_OS_H_

#include "types.h"

typedef Std_ReturnType (*BackgroundTask)(void);

uint32_t Os_GetCurrentTimeMs();
void     Os_Wait(uint32_t ms);

#endif /* OS_API_OS_H_ */
