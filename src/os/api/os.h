#ifndef __OS_API_H__
#define __OS_API_H__

#include "types.h"
#include "os_cfg.h"

// Background Tasks
typedef Std_ReturnType (*BackgroundTask)(void);
extern BackgroundTask BackgroundTasksList[NUMBER_OF_BACKGROUND_TASKS];

// App entry point
extern void App_Init(void);

void   Os_ResetTimer(Timer timer);
time_t Os_GetTimerValue(Timer timer);
void   Os_SetupTask(Timer timer, time_t interval, Callback callback, void* param);
void   Os_Wait(time_t ms);
void   Os_HardReset();


#endif /* __OS_API_H__ */
