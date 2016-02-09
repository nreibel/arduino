#ifndef SRC_TIMER_API_TIMER_H_
#define SRC_TIMER_API_TIMER_H_

#include "types.h"
#include "timer_cfg.h"

typedef enum {
	TIMER_NOT_OK = 0,
	TIMER_OK,
} TimerStatus;

TimerStatus Timer_Init(uint8 timerChannel, uint32 triggerValue, Callback cbk);
TimerStatus Timer_DeInit(uint8 timerChannel);

#endif /* SRC_TIMER_API_TIMER_H_ */
