#include "os.h"
#include "os_cfg.h"
#include "bits.h"
#include "types.h"
#include "string.h"

/*
 * Private types
 */

typedef struct {
    time_t value;
    bool running;
} timer_config_t;

typedef struct {
    time_t interval;
    time_t last;
    callback_t callback;
    void* param;
} task_config_t;

/*
 * Extern functions prototypes
 */

void os_init();
void os_sleep();
void os_interrupts_enable();
void os_interrupts_disable();

/*
 * Private functions prototypes
 */

static void os_cyclic_tasks();

#if NUMBER_OF_BACKGROUND_TASKS > 0
static int os_background_tasks();
#endif

/*
 * Private data
 */

static timer_config_t timers[NUMBER_OF_TIMERS] = {0};
static task_config_t tasks[NUMBER_OF_TASKS] = {0};

#if OS_MALLOC
static uint8_t malloc_heap[OS_HEAP_SIZE];
static uint8_t malloc_idx = 0;
#endif // OS_MALLOC

/*
 * Public functions
 */

void os_wait(time_t ms)
{
    if (ms <= 0) return;

    const time_t start = os_millis();

    while(1)
    {
        time_t elapsed = os_millis() - start;
        if (elapsed >= ms ) return;
        else os_sleep();
    }
}

void os_timer_start(timer_t timer)
{
    timers[timer].value = os_millis();
    timers[timer].running = TRUE;
}

void os_timer_reset(timer_t timer)
{
    timers[timer].value = os_millis();
}

void os_timer_stop(timer_t timer)
{
    timers[timer].value = os_millis() - timers[timer].value;
    timers[timer].running = FALSE;
}

void os_timer_resume(timer_t timer)
{
    timers[timer].value = os_millis() - timers[timer].value;
    timers[timer].running = TRUE;
}

time_t os_timer_value(timer_t timer)
{
    if (timers[timer].running) return os_millis() - timers[timer].value;
    else return timers[timer].value;
}

void os_task_setup(task_t task, time_t interval, callback_t callback, void* param)
{
    tasks[task].interval = interval;
    tasks[task].callback = callback;
    tasks[task].param = param;
    tasks[task].last = os_millis() - interval;
}

static void os_cyclic_tasks()
{
    for(int i = 0; i < NUMBER_OF_TASKS; i++)
    {
        if(tasks[i].interval <= 0 || tasks[i].callback == NULL_PTR) continue;

        time_t elapsed = os_millis() - tasks[i].last;

        if(elapsed >= tasks[i].interval)
        {
            tasks[i].callback(tasks[i].param);
            tasks[i].last += tasks[i].interval;
        }
    }
}

#if NUMBER_OF_BACKGROUND_TASKS > 0
static int os_background_tasks()
{
    int retval = 0;

    for ( int i = 0 ; i < NUMBER_OF_BACKGROUND_TASKS ; i++ )
    {
        if (background_tasks_list[i]() > 0)
        {
            retval = 1;
        }
        // TODO : handle task failed
    }

    return retval;
}
#endif

int main(void)
{
    /* Perform project-specific initialization */
    os_init();

    // Enable interrupts
    os_interrupts_enable();

    /* Initialization of the application */
    app_init();

    /* Run main loop */
    while(1)
    {

        os_cyclic_tasks();

#if NUMBER_OF_BACKGROUND_TASKS > 0
        // Execute background tasks in the spare time, or sleep until next tick
        if (os_background_tasks() == 0)
#endif
        {
            os_sleep();
        }
    }

    return 1;
}

#if OS_MALLOC

/*
 * Really basic implementation, will fail once heap is used
 */

void* os_calloc(unsigned int sz)
{
    void *ptr = os_malloc(sz);
    if (ptr != NULL_PTR) memset(ptr, 0, sz);
    return ptr;
}

void* os_malloc(unsigned int sz)
{
    void *ptr = NULL_PTR;

    if (malloc_idx + sz <= OS_HEAP_SIZE)
    {
        ptr = &malloc_heap[malloc_idx];
        malloc_idx += sz;
    }

    return ptr;
}

void os_free(void *ptr)
{
    UNUSED(ptr);
    // TODO
}

#endif // OS_MALLOC
