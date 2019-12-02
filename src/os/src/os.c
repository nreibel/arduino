#include "os.h"
#include "os_prv.h"
#include "os_cfg.h"
#include "app.h"
#include "timer.h"
#include "bits.h"
#include "port.h"

void Os_Wait(time_t ms)
{
    time_t begin = Os_GetCurrentTimeMs();
    while (Os_GetCurrentTimeMs() < begin + ms) Os_Sleep();
}

Std_ReturnType Os_ExecuteBackgroundTasks()
{
    Std_ReturnType retval = Status_OK;

    for (int i = 0 ; i < NUMBER_OF_BACKGROUND_TASKS ; i++)
    {
        if (BackgroundTasksList[i]() == Status_Pending)
        {
            retval = Status_Pending;
        }
    }

    return retval;
}

int main(void)
{  
    time_t lastTrigger = Os_GetCurrentTimeMs();
    
    /* Perform project-specific initialization */
    Os_Init();
 
    // Enable interrupts
    Os_EnableInterrupts();

    /* Initialize timer */
    Timer_CyclicTaskInit();

    /* Initialization of the application */
    App_Init();
    
    /* Run main loop */
    while(1)
    {
        // Check on cyclic tasks every ms
        time_t now = Os_GetCurrentTimeMs();
        if (now != lastTrigger)
        {
            Timer_CyclicTask();
            lastTrigger = now;
        }

#if NUMBER_OF_BACKGROUND_TASKS > 0
        // Execute background tasks in the spare time, or sleep until next tick
        if (Os_ExecuteBackgroundTasks() != Status_Pending)
#endif
        {
            Os_Sleep();
        }
    }

    return 1;
}
