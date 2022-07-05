#ifndef __TIMER16_HAL_API_H__
#define __TIMER16_HAL_API_H__

#include "types.h"
#include "tim16_ll.h"

typedef enum {
    TIMER1,
    TIMER3,
    NUMBER_OF_TIM16
} tim16_t;

enum {
    TIM16_OK,
    TIM16_ERROR_STATE,
    TIM16_ERROR_HANDLE,
    TIM16_ERROR_OVERFLOW,
    TIM16_ERROR_NO_DATA,
    TIM16_ERROR_PRESCALER,
    TIM16_ERROR_EDGE,
};

typedef enum {
    TIM16_PRESCALER_NONE,
    TIM16_PRESCALER_1,
    TIM16_PRESCALER_8,
    TIM16_PRESCALER_64,
    TIM16_PRESCALER_256,
    TIM16_PRESCALER_1024,
    NUMBER_OF_TIM16_PRESCALERS
} tim16_prescaler_t;

int tim16_init(tim16_t timer, tim16_prescaler_t pscl);
int tim16_start(tim16_t t);
int tim16_stop(tim16_t t);
int tim16_capture_input(tim16_t timer, unsigned int * period, uint8_t *duty_cycle);

#endif /* __TIMER16_HAL_API_H__ */
