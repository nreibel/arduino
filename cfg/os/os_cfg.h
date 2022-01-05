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

// Enable malloc and free
#define OS_MALLOC_NONE   0 // No malloc
#define OS_MALLOC_STDLIB 1 // RECOMMANDED : compiler/libc implementation of malloc
#define OS_MALLOC_SIMPLE 2 // Almost no overhead, but can't free memory
#define OS_MALLOC_CHUNKS 3 // Separate heap into chunks that can be freed, but uses RAM to manage chunks
#define OS_MALLOC OS_MALLOC_STDLIB

// Define heap size when using OS_MALLOC_SIMPLE and OS_MALLOC_CHUNKS
// #define OS_HEAP_SIZE 256

// When using OS_MALLOC_CHUNKS, increasing size of chunks decreases RAM usage, but increases fragmentation
// #define OS_HEAP_CHUNK_SIZE 8

// SLEEP_MODE_IDLE wakes up from most peripherals (Timer2, I2C, USART, ADC, IO...)
// SLEEP_MODE_PWR_SAVE only wakes up from I2C and Timer2 interrupts
#define OS_SLEEP_MODE SLEEP_MODE_IDLE

#define NUMBER_OF_BACKGROUND_TASKS 0

#define OS_ENABLE_PRINTF 1

typedef enum {
    TIMER_INTERVAL,
    NUMBER_OF_OS_TIMERS
} os_timer_t;

typedef enum {
    TASK_MAIN,
    NUMBER_OF_OS_TASKS
} os_task_t;

#endif /* SRC_OS_CFG_OS_CFG_H_ */
