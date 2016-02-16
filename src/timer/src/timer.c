#include "timer_prv.h"
#include "os.h"

/* Holds the status of each timer channel */
typedef struct {
	uint32_t   triggerTime;
	uint32_t   lastTrigger;
	Callback callback;
	boolean  channelEnabled;
} TimerConfig;

static TimerConfig timerCfg[NbrOfTimerChannels];

/* Initializes a specific timer channel with a trigger value and a callback each time the trigger expires */
TimerStatus Timer_Init(uint8_t timerChannel, uint32_t triggerValue, Callback cbk)
{
	TimerStatus status = TIMER_NOT_OK;

	if (timerChannel < NbrOfTimerChannels)
	{
		timerCfg[timerChannel].channelEnabled = TRUE;
		timerCfg[timerChannel].triggerTime    = triggerValue;
		timerCfg[timerChannel].lastTrigger    = Os_GetCurrentTimeMs();
		timerCfg[timerChannel].callback       = cbk;
		status = TIMER_OK;
	}

	return status;
}

/* Initialization of the cyclic task */
void Timer_CyclicTaskInit(void)
{
	uint8_t i;

	for (i = 0 ; i < NbrOfTimerChannels ; i++)
	{
		timerCfg[i].channelEnabled = FALSE;
		timerCfg[i].triggerTime    = 0;
		timerCfg[i].lastTrigger    = 0;
		timerCfg[i].callback       = NULL_PTR;
	}
}

/*
 * The cyclic functions checks every timer associated with a callback.
 * Timers not associated with a callback must be checked periodically.
 */
void Timer_CyclicTask(void)
{
	uint8_t i;
	boolean isElapsed = FALSE;
	TimerStatus retval = TIMER_NOT_OK;

	for (i = 0 ; i < NbrOfTimerChannels ; i++)
	{
		/* Only check the timer that have an automatic callback */
		if (timerCfg[i].channelEnabled && timerCfg[i].callback != NULL_PTR)
		{
			retval = Timer_IsChannelElapsed(i, &isElapsed);
			if (retval == TIMER_OK && isElapsed)
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
TimerStatus Timer_IsChannelElapsed(uint8_t channel, boolean *isElapsed)
{
	TimerStatus status = TIMER_NOT_OK;
	uint32_t currentTime = Os_GetCurrentTimeMs();
	uint32_t timeElapsed = 0;

	if (channel < NbrOfTimerChannels)
	{
		if (timerCfg[channel].channelEnabled)
		{
			*isElapsed = FALSE;

			// Handle timer overflow
			if (currentTime < timerCfg[channel].lastTrigger)
			{
				timeElapsed = currentTime + (0xFFFFFFFF - timerCfg[channel].lastTrigger) + 1;
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

			status = TIMER_OK;
		}
		else
		{
			status = TIMER_NOT_STARTED;
		}
	}

	return status;
}
