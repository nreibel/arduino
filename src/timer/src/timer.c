#include "types.h"
#include "timer.h"
#include "timer_prv.h"

/* Holds the status of each timer channel */
typedef struct {
	uint32 triggerValue;
	uint32 timeElapsed;
	Callback callback;
} TimerConfig;

static TimerConfig timerCfg[NbrOfTimerChannels];

/* Initializes a specific timer channel with a trigger value and a callback each time the trigger expires */
TimerStatus Timer_Init(uint8 timerChannel, uint32 triggerValue, Callback cbk)
{
	TimerStatus status = TIMER_NOT_OK;

	if (timerChannel < NbrOfTimerChannels)
	{
		timerCfg[timerChannel].triggerValue = triggerValue;
		timerCfg[timerChannel].timeElapsed = 0;
		timerCfg[timerChannel].callback = cbk;
		status = TIMER_OK;
	}

	return status;
}

/* Uninitializes a specific timer channel */
TimerStatus Timer_DeInit(uint8 timerChannel)
{
	return Timer_Init(timerChannel, 0, NULL_PTR);
}

/* Initialization of the cyclic task */
void Timer_CyclicTaskInit(void)
{
	int i;

	for (i = 0 ; i < NbrOfTimerChannels ; i++)
	{
		timerCfg[i].triggerValue = 0;
		timerCfg[i].timeElapsed = 0;
		timerCfg[i].callback = NULL_PTR;
	}
}

/* Cyclic task that updates each channels every millisecond */
void Timer_CyclicTask1ms(void)
{
	int i;

	for (i = 0 ; i < NbrOfTimerChannels ; i++)
	{
		if (timerCfg[i].triggerValue > 0)
		{
			if (timerCfg[i].timeElapsed >= timerCfg[i].triggerValue)
			{
				if (timerCfg[i].callback != NULL_PTR)
				{
					timerCfg[i].callback();
				}

				timerCfg[i].timeElapsed = 0;
			}
			else
			{
				timerCfg[i].timeElapsed++;
			}
		}
	}
}
