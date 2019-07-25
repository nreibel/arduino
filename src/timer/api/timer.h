#ifndef SRC_TIMER_API_TIMER_H_
#define SRC_TIMER_API_TIMER_H_

#include "types.h"
#include "timer_cfg.h"

void Timer_CyclicTaskInit(void);
void Timer_CyclicTask(void);

Std_ReturnType Timer_StartTask     (TimerChannel channel, uint32_t triggerTime, Callback cbk);
Std_ReturnType Timer_StartTimer    (TimerChannel channel, uint32_t triggerTime);
Std_ReturnType Timer_SetTriggerTime(TimerChannel channel, uint32_t triggerTime);
Std_ReturnType Timer_Enable        (TimerChannel channel);
Std_ReturnType Timer_Disable       (TimerChannel channel);
Std_ReturnType Timer_IsElapsed     (TimerChannel channel, boolean *isElapsed);

#endif /* SRC_TIMER_API_TIMER_H_ */
