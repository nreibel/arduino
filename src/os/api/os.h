#ifndef OS_API_OS_H_
#define OS_API_OS_H_

#include "types.h"
#include "timer_cfg.h"
#include "os_cfg.h"

typedef Std_ReturnType (*BackgroundTask)(void);

void Os_Init();
void Os_Sleep();

time_t Os_GetCurrentTimeMs();
void   Os_Wait(time_t ms);

void Os_EnableInterrupts();
void Os_DisableInterrupts();

#endif /* OS_API_OS_H_ */
