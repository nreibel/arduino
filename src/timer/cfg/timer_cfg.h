#ifndef SRC_TIMER_API_TIMER_CFG_H_
#define SRC_TIMER_API_TIMER_CFG_H_

// uint8_t has a max value of ~0,25s
// uint16_t has a max value of ~65s
// uint32_t has a max value of ~49,7 days
typedef uint16_t time_t;

typedef enum
{
    Timer_MainTask = 0,
    TIMER_NUMBER_OF_CHANNELS
} TimerChannel;

#endif /* SRC_TIMER_API_TIMER_H_ */
