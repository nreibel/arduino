#ifndef __TIMER16_HAL_API_H__
#define __TIMER16_HAL_API_H__

#include "types.h"
#include "timer16_ll.h"

typedef enum {
    TIMER1,
    TIMER3,
    NUMBER_OF_TIMER16
} timer16_t;

int tim16_init(timer16_t timer, tim16_prescaler_t pscl);
int tim16_capture_input(timer16_t timer, unsigned int * period, uint8_t *duty_cycle);
int tim16_capture_edge(timer16_t t, tim16_edge_t edge, uint16_t * value);

#endif /* __TIMER16_HAL_API_H__ */
