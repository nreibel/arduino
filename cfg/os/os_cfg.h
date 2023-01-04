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

// #define OS_RANDOM_SEED 0xAA

// Enable malloc and free
#define OS_MALLOC_NONE   0 // No malloc
#define OS_MALLOC_SIMPLE 1 // Almost no overhead, but can't free memory
#define OS_MALLOC_STDLIB 2 // Compiler/libc implementation of malloc
#define OS_MALLOC OS_MALLOC_SIMPLE

// Define heap size when using OS_MALLOC_SIMPLE
#define OS_HEAP_SIZE 256

// Let the CPU sleep when idle to reduce power consumption
// If the ADC is enabled, entering idle mode triggers a conversion!
#define OS_SLEEP_ON_IDLE ON

// Set all GPIO as inputs with pullup enabled to save power
#define OS_INIT_GPIO ON

#define OS_ENABLE_PRINTF ON

#define OS_CHECK_NULL    ON

typedef enum {
    NUMBER_OF_BACKGROUND_TASKS
} os_background_tasks_t;

typedef enum {
    NUMBER_OF_OS_TIMERS
} os_timer_t;

typedef enum {
    TASK_MAIN,
    NUMBER_OF_OS_TASKS
} os_task_t;

#endif /* SRC_OS_CFG_OS_CFG_H_ */
