#include "os_cfg.h"
#include "timer16.h"
#include "timer16_ll.h"

#include <avr/power.h>

static struct {
    const mem_tim16_t tim;
    const tim16_tifr_t tifr;
    uint8_t factor;
} __timers[NUMBER_OF_TIMER16] = {
    [TIMER1] = {
        .tim = TIM1,
        .tifr = _TIFR1,
    },
    [TIMER3] = {
        .tim = TIM3,
        .tifr = _TIFR3,
    },
};

enum {
    TIM16_OK,
    TIM16_ERROR_HANDLE,
    TIM16_ERROR_OVERFLOW,
    TIM16_ERROR_PRESCALER,
    TIM16_ERROR_EDGE,
};

int tim16_init(timer16_t timer, tim16_prescaler_t pscl)
{
    switch(pscl)
    {
        case TIM16_LL_PRESCALER_1:    __timers[timer].factor = 0;  break; // 2^0
        case TIM16_LL_PRESCALER_8:    __timers[timer].factor = 3;  break; // 2^3
        case TIM16_LL_PRESCALER_64:   __timers[timer].factor = 6;  break; // 2^6
        case TIM16_LL_PRESCALER_256:  __timers[timer].factor = 8;  break; // 2^8
        case TIM16_LL_PRESCALER_1024: __timers[timer].factor = 10; break; // 2^10
        default: return -TIM16_ERROR_PRESCALER;
    }

    switch(timer)
    {
        case TIMER1: power_timer1_enable(); break;
        case TIMER3: power_timer3_enable(); break;
        default: return -TIM16_ERROR_HANDLE;
    }

    tim16_ll_set_prescaler(__timers[timer].tim, pscl);

    return TIM16_OK;
}

int tim16_capture_input(timer16_t timer, unsigned int * period, uint8_t *duty_cycle)
{
    int ret = 0;
    uint16_t t1, t2, t3;

    if (timer >= NUMBER_OF_TIMER16)
        return -TIM16_ERROR_HANDLE;

    tim16_ll_reset_ovf(__timers[timer].tifr);
    tim16_ll_reset_value(__timers[timer].tim);

    ret += tim16_capture_edge(timer, TIM16_LL_RISING_EDGE,  &t1);
    ret += tim16_capture_edge(timer, TIM16_LL_FALLING_EDGE, &t2);
    ret += tim16_capture_edge(timer, TIM16_LL_RISING_EDGE,  &t3);

    if (ret < 0)
        return -TIM16_ERROR_OVERFLOW;

    int total = t3 - t1;
    int high = t2 - t1;

    *period = (F_CPU >> __timers[timer].factor) / total;
    *duty_cycle = (255ul * high) / total;

    return TIM16_OK;
}

int tim16_capture_edge(timer16_t timer, tim16_edge_t edge, uint16_t * value)
{
    if (timer >= NUMBER_OF_TIMER16)
        return -TIM16_ERROR_HANDLE;

    switch(edge)
    {
        case TIM16_LL_RISING_EDGE:
        case TIM16_LL_FALLING_EDGE:
            break;

        default:
            return -TIM16_ERROR_EDGE;
    }

    tim16_ll_select_input_capture_edge(__timers[timer].tim, edge);
    tim16_ll_reset_icf(__timers[timer].tifr);

    while ( !tim16_ll_check_icf(__timers[timer].tifr) )
        if ( tim16_ll_check_ovf(__timers[timer].tifr) )
            return -TIM16_ERROR_OVERFLOW;

    *value = tim16_ll_get_input_capture(__timers[timer].tim);

    return TIM16_OK;
}
