#include "timer.h"
#include "timer_prv.h"
#include "timer_cfg.h"
#include "os.h"

static TimerConfig timerCfg[TIMER_NUMBER_OF_CHANNELS];

/* Initialization of the cyclic task */
void Timer_CyclicTaskInit(void)
{
    uint8_t i;

    for (i = 0 ; i < TIMER_NUMBER_OF_CHANNELS ; i++)
    {
        timerCfg[i].status      = Uninitialized;
        timerCfg[i].triggerTime = 0;
        timerCfg[i].lastTrigger = 0;
        timerCfg[i].callback    = NULL_PTR;
    }
}

/* Initializes a specific timer channel with a trigger value and a callback each time the trigger expires */
Std_ReturnType Timer_StartTask(TimerChannel channel, time_t triggerValue, Callback cbk)
{
    Std_ReturnType status = Status_Not_OK;

    if (channel < TIMER_NUMBER_OF_CHANNELS)
    {
        timerCfg[channel].status      = Enabled;
        timerCfg[channel].triggerTime = triggerValue;
        timerCfg[channel].lastTrigger = Os_GetCurrentTimeMs();
        timerCfg[channel].callback    = cbk;
        status = Status_OK;
    }

    return status;
}

/* Initializes a specific timer channel with a trigger value */
Std_ReturnType Timer_StartTimer(TimerChannel channel, time_t triggerValue)
{
    Std_ReturnType status = Status_Not_OK;

    if (channel < TIMER_NUMBER_OF_CHANNELS)
    {
        timerCfg[channel].status      = Enabled;
        timerCfg[channel].triggerTime = triggerValue;
        timerCfg[channel].lastTrigger = Os_GetCurrentTimeMs();
        timerCfg[channel].callback    = NULL_PTR;
        status = Status_OK;
    }

    return status;
}

/* Reset a specific timer channel */
Std_ReturnType Timer_ResetTimer(TimerChannel channel)
{
    Std_ReturnType status = Status_Not_OK;

    if (channel < TIMER_NUMBER_OF_CHANNELS)
    {
        timerCfg[channel].lastTrigger = Os_GetCurrentTimeMs();
        status = Status_OK;
    }

    return status;
}

Std_ReturnType Timer_Enable(TimerChannel channel)
{
    Std_ReturnType status = Status_Not_OK;

    if (channel < TIMER_NUMBER_OF_CHANNELS)
    {
        if (timerCfg[channel].status != Uninitialized)
        {
            timerCfg[channel].status = Enabled;
            status = Status_OK;
        }
    }

    return status;
}

Std_ReturnType Timer_IsEnabled(TimerChannel channel, boolean *isEnabled)
{
    Std_ReturnType status = Status_Not_OK;
    if (channel < TIMER_NUMBER_OF_CHANNELS)
    {
        *isEnabled = FALSE;
        if (timerCfg[channel].status == Enabled)
        {
            *isEnabled = TRUE;
        }
        status = Status_OK;
    }

    return status;
}

Std_ReturnType Timer_Disable(TimerChannel channel)
{
    Std_ReturnType status = Status_Not_OK;

    if (channel < TIMER_NUMBER_OF_CHANNELS)
    {
        if (timerCfg[channel].status != Uninitialized)
        {
            timerCfg[channel].status = Disabled;
            status = Status_OK;
        }
    }

    return status;
}

Std_ReturnType Timer_SetTriggerTime(TimerChannel channel, time_t triggerTime)
{
    Std_ReturnType status = Status_Not_OK;

    if (channel < TIMER_NUMBER_OF_CHANNELS)
    {
        if (timerCfg[channel].status != Uninitialized)
        {
            timerCfg[channel].triggerTime = triggerTime;
            status = Status_OK;
        }
    }

    return status;
}

/*
* The cyclic functions checks every timer associated with a callback.
* Timers not associated with a callback must be checked periodically.
*/
void Timer_CyclicTask(void)
{
    boolean isElapsed = FALSE;

    for (TimerChannel i = 0 ; i < TIMER_NUMBER_OF_CHANNELS ; i++)
    {
        /* Only check the timer that have an automatic callback */
        if (timerCfg[i].status == Enabled && timerCfg[i].callback != NULL_PTR)
        {
            if (Timer_IsElapsed(i, &isElapsed) == Status_OK && isElapsed)
            {
                timerCfg[i].callback();
            }
        }
    }
}

/*
* Checks if a timer has expired.
* If the timer has expired and this function is called, it is re-triggered.
* Do NOT call this function with a channel timer used for a callback, it might re-trigger the timer without executing the callback
*/
Std_ReturnType Timer_IsElapsed(TimerChannel channel, boolean *isElapsed)
{
    Std_ReturnType status = Status_Not_OK;
    time_t currentTime = Os_GetCurrentTimeMs();
    time_t timeElapsed = 0;

    if (channel < TIMER_NUMBER_OF_CHANNELS)
    {
        if (timerCfg[channel].status == Enabled)
        {
            *isElapsed = FALSE;

            // Handle timer overflow
            if (currentTime < timerCfg[channel].lastTrigger)
            {
                timeElapsed = currentTime + ( TYPECAST(UINT32_MAX, time_t) - timerCfg[channel].lastTrigger) + 1;
            }
            else
            {
                timeElapsed = currentTime - timerCfg[channel].lastTrigger;
            }

            // If timer elapsed, trigger it again by setting the last trigger time to the current time
            if (timeElapsed >= timerCfg[channel].triggerTime)
            {
                timerCfg[channel].lastTrigger = currentTime;
                *isElapsed = TRUE;
            }

            status = Status_OK;
        }
    }

    return status;
}
