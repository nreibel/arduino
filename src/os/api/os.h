#ifndef OS_API_OS_H_
#define OS_API_OS_H_

#include "types.h"

uint32_t Os_GetCurrentTimeMs();
void     Os_Sleep(uint32_t ms);
void     Os_EnableInterrupts();

#endif /* OS_API_OS_H_ */
