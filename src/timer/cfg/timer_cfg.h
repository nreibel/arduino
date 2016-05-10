#ifndef SRC_TIMER_API_TIMER_CFG_H_
#define SRC_TIMER_API_TIMER_CFG_H_

#include "app_cfg.h"

typedef enum
{
	Timer_MainTask = 0,
	Timer_ButtonPress,
	Timer_LedBlink,
	NbrOfTimerChannels
} TimerChannel;

#endif /* SRC_TIMER_API_TIMER_H_ */
