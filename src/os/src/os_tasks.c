#include "os.h"
#include "os_cfg.h"

/*
 * Private data
 */

static struct {
    time_t interval;
    time_t last;
    callback_t callback;
    void* param;
} tasks[NUMBER_OF_OS_TASKS];

/*
 * Public functions
 */

void os_task_setup(os_task_t task, time_t interval, callback_t callback, void* param)
{
    tasks[task].interval = interval;
    tasks[task].callback = callback;
    tasks[task].param = param;
    tasks[task].last = os_millis() - interval;
}

void os_cyclic_tasks()
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

int os_background_tasks()
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
