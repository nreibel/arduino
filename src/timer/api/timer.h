#ifndef SRC_TIMER_API_TIMER_H_
#define SRC_TIMER_API_TIMER_H_

#include "types.h"

typedef enum {
	TIMER_NOT_OK = 0,
	TIMER_OK,
	TIMER_NOT_STARTED,
} TimerStatus;

TimerStatus Timer_Init(uint8 timerChannel, uint32 triggerValue, Callback cbk);
TimerStatus Timer_IsChannelElapsed(uint8 channel, boolean *isElapsed);

#endif /* SRC_TIMER_API_TIMER_H_ */
