#ifndef __OS_API_H__
#define __OS_API_H__

#include "types.h"
#include "os_cfg.h"

#if OS_ENABLE_PRINTF
#include "stdio.h"
#endif // OS_ENABLE_PRINTF

#define HALT for(;;){}

typedef int (*callback_t)(void*);

enum {
    EOK,
    EDONE,
    EPENDING,
    EFAILED
};

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

// Enable/disable interrupts
void os_interrupts_enable();
void os_interrupts_disable();

// OS Timers
void os_wait(time_t ms);
void os_wait_us(unsigned int us);
void os_timer_start(os_timer_t timer);
void os_timer_reset(os_timer_t timer);
void os_timer_stop(os_timer_t timer);
void os_timer_resume(os_timer_t timer);
time_t os_timer_value(os_timer_t timer);
time_t os_millis();

// OS Tasks
void os_task_setup(os_task_t task, time_t interval, callback_t callback, void* param);
void os_task_enable(os_task_t task);
void os_task_disable(os_task_t task);

// HW Watchdog
int os_watchdog_enable(os_watchdog_t wd);
void os_watchdog_trigger();

// OS Functions
void os_powerdown();
void os_reset();
void os_systick();

uint32_t os_read_signature_row();

#ifdef OS_RANDOM_SEED
uint8_t os_rand();
#endif // OS_RANDOM_SEED

#if OS_ENABLE_PRINTF
extern int os_putc(char character, FILE *stream);
#endif // OS_ENABLE_PRINTF

#endif /* __OS_API_H__ */
