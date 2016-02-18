#ifndef SRC_TIMER_API_TIMER_CFG_H_
#define SRC_TIMER_API_TIMER_CFG_H_

typedef enum
{
	Timer_MainTask = 0,
	Timer_BlinkTask,
	Timer_BuzzTask,
	NbrOfTimerChannels
} TimerChannel;

#endif /* SRC_TIMER_API_TIMER_H_ */
