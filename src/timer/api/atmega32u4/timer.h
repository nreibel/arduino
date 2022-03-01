#ifndef TIMER_API_H__
#define TIMER_API_H__

#include "timer_ll.h"

typedef enum {
    TIMER0,
    NUMBER_OF_TIMERS
} timer_t;

#define TIM0 TYPECAST(0x44, const mem_timer_t)

#endif /* TIMER_API_H__ */
