#ifndef SRC_OS_CFG_OS_CFG_H_
#define SRC_OS_CFG_OS_CFG_H_

#include "types.h"

// uint8_t has a max value of ~0,25s
// uint16_t has a max value of ~65s
// uint32_t has a max value of ~49,7 days
typedef uint16_t time_t;

// In Hz
#define F_CPU 16000000UL

// In ms
#define OS_TIMER_GRANULARITY 1

#define OS_SLEEP_MODE SLEEP_MODE_IDLE

#define NUMBER_OF_BACKGROUND_TASKS 0

typedef enum {
    TIMER_MAIN,
    NUMBER_OF_TIMERS
} timer_t;

#endif /* SRC_OS_CFG_OS_CFG_H_ */
