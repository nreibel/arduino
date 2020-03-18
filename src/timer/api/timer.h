#ifndef SRC_TIMER_API_TIMER_H_
#define SRC_TIMER_API_TIMER_H_

#include "types.h"
#include "timer_cfg.h"

void Timer_CyclicTaskInit(void);
void Timer_CyclicTask(void);

Std_ReturnType Timer_StartTask     (TimerChannel channel, time_t triggerTime, Callback cbk, void* param);
Std_ReturnType Timer_StartTimer    (TimerChannel channel, time_t triggerTime);
Std_ReturnType Timer_SetTriggerTime(TimerChannel channel, time_t triggerTime);
Std_ReturnType Timer_ResetTimer    (TimerChannel channel);
Std_ReturnType Timer_Enable        (TimerChannel channel);
Std_ReturnType Timer_Disable       (TimerChannel channel);
Std_ReturnType Timer_IsEnabled     (TimerChannel channel, bool *isEnabled);
Std_ReturnType Timer_IsElapsed     (TimerChannel channel, bool *isElapsed);

#endif /* SRC_TIMER_API_TIMER_H_ */
