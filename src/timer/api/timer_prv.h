#ifndef SRC_TIMER_API_TIMER_PRV_H_
#define SRC_TIMER_API_TIMER_PRV_H_

#include "types.h"

typedef enum {
	Uninitialized,
	Disabled,
	Enabled
} TimerStatus;

/* Holds the status of each timer channel */
typedef struct {
	time_t    triggerTime;
	time_t    lastTrigger;
	Callback    callback;
	TimerStatus status;
} TimerConfig;

#endif /* SRC_TIMER_API_TIMER_PRV_H_ */
