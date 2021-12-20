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
} os_watchdog_t;

// Background Tasks
typedef int (*background_task)(void);
extern background_task background_tasks_list[NUMBER_OF_BACKGROUND_TASKS];

// App entry point
extern void app_init(void);

// OS Timers
void os_wait(time_t ms);
void os_timer_start(timer_t timer);
void os_timer_reset(timer_t timer);
void os_timer_stop(timer_t timer);
void os_timer_resume(timer_t timer);
time_t os_timer_value(timer_t timer);
time_t os_millis();

// OS Tasks
void os_task_setup(task_t task, time_t interval, callback_t callback, void* param);

// HW Watchdog
int os_watchdog_enable(os_watchdog_t wd);
void os_watchdog_trigger();

// OS Functions
void os_reset();

// Memory Allocation
#if OS_MALLOC
void* os_malloc(unsigned int sz);
void* os_calloc(unsigned int sz);
void  os_free(void *ptr);
unsigned int os_get_used_heap();
unsigned int os_get_free_heap();
void os_heap_full();
#endif // OS_MALLOC

#endif /* __OS_API_H__ */
