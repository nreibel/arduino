#ifndef __OS_API_H__
#define __OS_API_H__

#include "types.h"
#include "os_cfg.h"

typedef enum {
    OS_WATCHDOG_15MS,
    OS_WATCHDOG_30MS,
    OS_WATCHDOG_60MS,
    OS_WATCHDOG_120MS,
    OS_WATCHDOG_250MS,
    OS_WATCHDOG_500MS,
    OS_WATCHDOG_1S,
    OS_WATCHDOG_2S,
    OS_WATCHDOG_4S,
    OS_WATCHDOG_8S,
} Os_Watchdog;

// Background Tasks
typedef Std_ReturnType (*BackgroundTask)(void);
extern BackgroundTask BackgroundTasksList[NUMBER_OF_BACKGROUND_TASKS];

// App entry point
extern void App_Init(void);

void Os_ResetTimer(Timer timer);
time_t Os_GetTimerValue(Timer timer);
void Os_SetupTask(Timer timer, time_t interval, Callback callback, void* param);
void Os_Wait(time_t ms);
void Os_HardReset();
void Os_ResetWatchdog();
int Os_EnableWatchdog(Os_Watchdog wd);

#endif /* __OS_API_H__ */
