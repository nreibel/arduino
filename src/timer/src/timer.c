#include "timer.h"
#include "timer_prv.h"
#include "timer_cfg.h"
#include "os.h"

typedef enum {
	Uninitialized,
	Disabled,
	Enabled
} TimerStatus;

/* Holds the status of each timer channel */
typedef struct {
	uint32_t    triggerTime;
	uint32_t    lastTrigger;
	Callback    callback;
	TimerStatus status;
} TimerConfig;

static TimerConfig timerCfg[NbrOfTimerChannels];

/* Initialization of the cyclic task */
void Timer_CyclicTaskInit(void)
{
	uint8_t i;

	for (i = 0 ; i < NbrOfTimerChannels ; i++)
	{
		timerCfg[i].status      = Uninitialized;
		timerCfg[i].triggerTime = 0;
		timerCfg[i].lastTrigger = 0;
		timerCfg[i].callback    = NULL_PTR;
	}
}

/* Initializes a specific timer channel with a trigger value and a callback each time the trigger expires */
Std_ReturnType Timer_InitTask(TimerChannel channel, uint32_t triggerValue, Callback cbk)
{
	Std_ReturnType status = Status_Not_OK;

	if (channel < NbrOfTimerChannels)
	{
		timerCfg[channel].status      = Disabled;
		timerCfg[channel].triggerTime = triggerValue;
		timerCfg[channel].lastTrigger = Os_GetCurrentTimeMs();
		timerCfg[channel].callback    = cbk;
		status = Status_OK;
	}

	return status;
}

Std_ReturnType Timer_Enable(TimerChannel channel)
{
	Std_ReturnType status = Status_Not_OK;

	if (channel < NbrOfTimerChannels)
	{
		if (timerCfg[channel].status != Uninitialized)
		{
			timerCfg[channel].status = Enabled;
			status = Status_OK;
		}
	}

	return status;
}

Std_ReturnType Timer_Disable(TimerChannel channel)
{
	Std_ReturnType status = Status_Not_OK;

	if (channel < NbrOfTimerChannels)
	{
		if (timerCfg[channel].status != Uninitialized)
		{
			timerCfg[channel].status = Disabled;
			status = Status_OK;
		}
	}

	return status;
}

Std_ReturnType Timer_SetTriggerTime(TimerChannel channel, uint32_t triggerTime)
{
	Std_ReturnType status = Status_Not_OK;

	if (channel < NbrOfTimerChannels)
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
	TimerChannel i;
	boolean isElapsed = FALSE;
	Std_ReturnType retval = Status_Not_OK;

	for (i = 0 ; i < NbrOfTimerChannels ; i++)
	{
		/* Only check the timer that have an automatic callback */
		if (timerCfg[i].status == Enabled && timerCfg[i].callback != NULL_PTR)
		{
			retval = Timer_IsElapsed(i, &isElapsed);
			if (retval == Status_OK && isElapsed)
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
	uint32_t currentTime = Os_GetCurrentTimeMs();
	uint32_t timeElapsed = 0;

	if (channel < NbrOfTimerChannels)
	{
		if (timerCfg[channel].status == Enabled)
		{
			*isElapsed = FALSE;

			// Handle timer overflow
			if (currentTime < timerCfg[channel].lastTrigger)
			{
				timeElapsed = currentTime + (UINT32_MAX - timerCfg[channel].lastTrigger) + 1;
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
