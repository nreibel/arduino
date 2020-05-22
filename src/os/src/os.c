#include "os.h"
#include "os_prv.h"
#include "os_cfg.h"
#include "app.h"
#include "bits.h"
#include "port.h"

volatile time_t osTimer = {0};
TimerConfig timerCfg[NUMBER_OF_TIMERS] = {0};

void Os_Wait(time_t ms)
{
    osTimer = 0;
    while (osTimer < ms)
    {
        Os_Sleep();
    }
}

void Os_ResetTimer(Timer timer)
{
    timerCfg[timer].value = 0;
}

time_t Os_GetTimerValue(Timer timer)
{
    return timerCfg[timer].value;
}

void Os_SetupTask(Timer timer, time_t interval, Callback callback, void* param)
{
    timerCfg[timer].interval = interval;
    timerCfg[timer].callback = callback;
    timerCfg[timer].param = param;
    timerCfg[timer].value = 0;
}

void Os_CyclicTasks()
{
    for (int i = 0 ; i < NUMBER_OF_TIMERS ; i++ )
    {
        if ( timerCfg[i].interval > 0 && timerCfg[i].callback != NULL_PTR && timerCfg[i].value >= timerCfg[i].interval )
        {
            timerCfg[i].callback( timerCfg[i].param );
            timerCfg[i].value %= timerCfg[i].interval; // TODO : avoid modulo
        }
    }
}

Std_ReturnType Os_ExecuteBackgroundTasks()
{
    Std_ReturnType retval = Status_OK;

    for ( int i = 0 ; i < NUMBER_OF_BACKGROUND_TASKS ; i++ )
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
    /* Perform project-specific initialization */
    Os_Init();

    // Enable interrupts
    Os_EnableInterrupts();

    /* Initialization of the application */
    App_Init();

    /* Run main loop */
    while(1)
    {

        Os_CyclicTasks();

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
