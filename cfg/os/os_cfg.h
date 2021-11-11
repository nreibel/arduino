#ifndef SRC_OS_CFG_OS_CFG_H_
#define SRC_OS_CFG_OS_CFG_H_

#include "types.h"

// uint8_t has a max value of ~0,25s
// uint16_t has a max value of ~65s
// uint32_t has a max value of ~49,7 days
// uint64_t has a max value of ~656 935 330 years
typedef uint16_t time_t;

// In Hz
#define F_CPU 16000000UL

// Enable malloc and free (not recommended)
#define OS_MALLOC ON

#if OS_MALLOC
#define OS_HEAP_SIZE 64
#endif // OS_MALLOC

#define OS_SLEEP_MODE SLEEP_MODE_IDLE

#define NUMBER_OF_BACKGROUND_TASKS 0

typedef enum {
    TIMER_INTERVAL,
    NUMBER_OF_TIMERS
} timer_t;

typedef enum {
    TASK_MAIN,
    NUMBER_OF_TASKS
} task_t;

#endif /* SRC_OS_CFG_OS_CFG_H_ */
