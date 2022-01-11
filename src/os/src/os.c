#include "os.h"
#include "os_cfg.h"
#include "bits.h"
#include "types.h"
#include "string.h"

#if OS_ENABLE_PRINTF
#include "stdio.h"
#endif // OS_ENABLE_PRINTF

/*
 * Extern functions prototypes
 */

void os_init();
void os_sleep();

/*
 * Private functions prototypes
 */

static void os_cyclic_tasks();
static int os_background_tasks();

/*
 * Private data
 */

static struct {
    time_t value;
    bool running;
} timers[NUMBER_OF_OS_TIMERS];

static struct {
    time_t interval;
    time_t last;
    callback_t callback;
    void* param;
} tasks[NUMBER_OF_OS_TASKS];

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

void os_timer_start(os_timer_t timer)
{
    timers[timer].value = os_millis();
    timers[timer].running = TRUE;
}

void os_timer_reset(os_timer_t timer)
{
    timers[timer].value = os_millis();
}

void os_timer_stop(os_timer_t timer)
{
    timers[timer].value = os_millis() - timers[timer].value;
    timers[timer].running = FALSE;
}

void os_timer_resume(os_timer_t timer)
{
    timers[timer].value = os_millis() - timers[timer].value;
    timers[timer].running = TRUE;
}

time_t os_timer_value(os_timer_t timer)
{
    if (timers[timer].running) return os_millis() - timers[timer].value;
    else return timers[timer].value;
}

void os_task_setup(os_task_t task, time_t interval, callback_t callback, void* param)
{
    tasks[task].interval = interval;
    tasks[task].callback = callback;
    tasks[task].param = param;
    tasks[task].last = os_millis() - interval;
}

static void os_cyclic_tasks()
{
    for(int i = 0; i < NUMBER_OF_OS_TASKS; i++)
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

static int os_background_tasks()
{
    int retval = 0;

    for ( int i = 0 ; i < NUMBER_OF_BACKGROUND_TASKS ; i++ )
    {
        if (background_tasks_list[i]() < 0)
        {
            retval = -1;
        }
        // TODO : handle task failed
    }

    return retval;
}

#if OS_ENABLE_PRINTF
__attribute__((weak)) int os_putc(char character, FILE *stream)
{
    UNUSED(stream);
    UNUSED(character);
    return EOF;
}

__attribute__((weak)) int os_getc(FILE *stream)
{
    UNUSED(stream);
    return EOF;
}

static FILE os_stdout = FDEV_SETUP_STREAM(os_putc, os_getc, _FDEV_SETUP_RW);
#endif // OS_ENABLE_PRINTF


int main(void)
{
    /* Perform project-specific initialization */
    os_init();

#if OS_ENABLE_PRINTF
    stdout = &os_stdout;
#endif // OS_ENABLE_PRINTF

    // Enable interrupts
    os_interrupts_enable();

    /* Initialization of the application */
    app_init();

    /* Run main loop */
    while(1)
    {

        os_cyclic_tasks();

        // Execute background tasks in the spare time, or sleep until next tick
        if (os_background_tasks() == 0)
        {
            os_sleep();
        }
    }

    return 1;
}
