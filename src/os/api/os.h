#ifndef OS_API_OS_H_
#define OS_API_OS_H_

#include "types.h"
#include "os_cfg.h"

typedef Std_ReturnType (*BackgroundTask)(void);

void   Os_ResetTimer(Timer timer);
time_t Os_GetTimerValue(Timer timer);
void   Os_SetupTask(Timer timer, time_t interval, Callback callback, void* param);
void   Os_Wait(time_t ms);
void   Os_EnableInterrupts();
void   Os_DisableInterrupts();
void   Os_HardReset();

extern void App_Init(void);

#endif /* OS_API_OS_H_ */
