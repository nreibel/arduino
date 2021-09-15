#ifndef SRC_OS_API_OS_PRV_H_
#define SRC_OS_API_OS_PRV_H_

#include "types.h"
#include "os_cfg.h"

/* Holds the status of each timer channel */
typedef struct {
    volatile time_t value;
    time_t interval;
    Callback callback;
    void* param;
    TaskState_t state;
} TimerConfig;

extern BackgroundTask BackgroundTasksList[NUMBER_OF_BACKGROUND_TASKS];
extern volatile time_t counters[NUMBER_OF_TIMERS];

void Os_Init();
void Os_Sleep();
Std_ReturnType Os_ExecuteBackgroundTasks();

#endif /* SRC_OS_API_OS_PRV_H_ */
