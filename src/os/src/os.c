#include "os.h"
#include "os_prv.h"
#include "os_cfg.h"
#include "bits.h"
#include "string.h"

#if NUMBER_OF_BACKGROUND_TASKS > 0
static int execute_background_tasks();
#endif

/* Holds the status of each timer channel */
typedef struct {
    time_t interval;
    volatile time_t value;
    callback_t callback;
    void* param;
} timer_config_t;

static volatile time_t os_timer = {0};
static timer_config_t timer_cfg[NUMBER_OF_TIMERS] = {0};

void os_wait(time_t ms)
{
    os_timer = 0;
    while (os_timer < ms)
    {
        os_sleep();
    }
}

void os_timer_callback()
{
    os_timer += OS_TIMER_GRANULARITY;
    for ( int i = 0 ; i < NUMBER_OF_TIMERS ; i++ )
    {
        timer_cfg[i].value += OS_TIMER_GRANULARITY;
    }
}

void os_timer_reset(timer_t timer)
{
    timer_cfg[timer].value = 0;
}

time_t os_timer_get_value(timer_t timer)
{
    return timer_cfg[timer].value;
}

void os_task_setup(timer_t timer, time_t interval, callback_t callback, void* param)
{
    timer_cfg[timer].interval = interval;
    timer_cfg[timer].callback = callback;
    timer_cfg[timer].param = param;
    timer_cfg[timer].value = interval;
}

void Os_CyclicTasks()
{
    for (int i = 0 ; i < NUMBER_OF_TIMERS ; i++ )
    {
        if ( timer_cfg[i].interval > 0 && timer_cfg[i].callback != NULL_PTR && timer_cfg[i].value >= timer_cfg[i].interval )
        {
            timer_cfg[i].callback( timer_cfg[i].param );
            timer_cfg[i].value %= timer_cfg[i].interval;
        }
    }
}

#if NUMBER_OF_BACKGROUND_TASKS > 0
static int execute_background_tasks()
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

        Os_CyclicTasks();

#if NUMBER_OF_BACKGROUND_TASKS > 0
        // Execute background tasks in the spare time, or sleep until next tick
        if (execute_background_tasks() == 0)
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

static uint8_t malloc_heap[OS_HEAP_SIZE];
static uint8_t malloc_idx = 0;

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
