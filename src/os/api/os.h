#ifndef OS_API_OS_H_
#define OS_API_OS_H_

#include "types.h"
#include "os_cfg.h"

typedef enum TaskState_e {
    TASK_STATE_NONE,
    TASK_STATE_ACTIVE,
    TASK_STATE_SUSPENDED
} TaskState_t;

typedef Std_ReturnType (*BackgroundTask)(void);

// Timers
void Os_ResetTimer(Timer timer);
time_t Os_GetTimerValue(Timer timer);

// Tasks
TaskState_t Os_GetTaskState(Timer timer);
void Os_SetupTask(Timer timer, time_t interval, Callback callback, void* param);
void Os_SuspendTask(Timer timer);
void Os_ResumeTask(Timer timer);

// OS Functions
void Os_Wait(time_t ms);
void Os_EnableInterrupts();
void Os_DisableInterrupts();
void Os_HardReset();

extern void App_Init(void);

#endif /* OS_API_OS_H_ */
