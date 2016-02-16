#ifndef SRC_TIMER_API_TIMER_H_
#define SRC_TIMER_API_TIMER_H_

#include "types.h"
#include "timer_cfg.h"

Std_ReturnType Timer_Init(TimerChannel timerChannel, uint32_t triggerValue, Callback cbk);
Std_ReturnType Timer_IsChannelElapsed(TimerChannel channel, boolean *isElapsed);

#endif /* SRC_TIMER_API_TIMER_H_ */
