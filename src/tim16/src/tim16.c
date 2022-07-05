#include "os_cfg.h"
#include "tim16.h"
#include "tim16_ll.h"

#include <string.h>
#include <avr/power.h>

typedef enum {
    TIM16_RESET = 0,
    TIM16_STOPPED,
    TIM16_RUNNING,
} tim16_state_t;

static struct {
    const mem_tim16_t tim;
    const tim16_tifr_t tifr;
    const tim16_timsk_t timsk;
    tim16_state_t state;
    tim16_ll_pscl_t pscl;
    uint8_t ovf;
    uint16_t t1;
    uint16_t t2;
    uint16_t th;
    uint8_t cpt;
} _timers[NUMBER_OF_TIM16] = {
    [TIMER1] = {
        .state = TIM16_RESET,
        .tim   = _TIM1,
        .tifr  = _TIFR1,
        .timsk = _TIMSK1,
    },
    [TIMER3] = {
        .state = TIM16_RESET,
        .tim   = _TIM3,
        .tifr  = _TIFR3,
        .timsk = _TIMSK3,
    },
};

void tim16_ll_icp_cbk(mem_tim16_t timer, tim16_edge_t edge, uint16_t value)
{
    for (tim16_t t = 0 ; t < NUMBER_OF_TIM16 ; t++)
    {
        if (_timers[t].tim == timer)
        {
            switch(edge)
            {
                case TIM16_LL_RISING_EDGE:
                    _timers[t].t1 = _timers[t].t2;
                    _timers[t].t2 = value;
                    break;

                case TIM16_LL_FALLING_EDGE:
                    _timers[t].th = value;
                    break;

                default:
                    break;
            }

            if (_timers[t].cpt++ < 0xFF)
                _timers[t].cpt++;

            _timers[t].ovf = 0;
        }
    }
}

void tim16_ll_ovf_cbk(mem_tim16_t timer)
{
    for (tim16_t t = 0 ; t < NUMBER_OF_TIM16 ; t++)
        if (_timers[t].tim == timer && _timers[t].ovf < 0xFF)
            _timers[t].ovf++;
}

int tim16_init(tim16_t timer, tim16_prescaler_t pscl)
{
    tim16_ll_pscl_t p = TIM16_LL_PSCL_NONE;

    switch(pscl)
    {
        case TIM16_PRESCALER_1:    p = TIM16_LL_PSCL_1;    break;
        case TIM16_PRESCALER_8:    p = TIM16_LL_PSCL_8;    break;
        case TIM16_PRESCALER_64:   p = TIM16_LL_PSCL_64;   break;
        case TIM16_PRESCALER_256:  p = TIM16_LL_PSCL_256;  break;
        case TIM16_PRESCALER_1024: p = TIM16_LL_PSCL_1024; break;
        default: return -TIM16_ERROR_PRESCALER;
    }

    switch(timer)
    {
        case TIMER1: power_timer1_enable(); break;
        case TIMER3: power_timer3_enable(); break;
        default: return -TIM16_ERROR_HANDLE;
    }

    _timers[timer].state = TIM16_STOPPED;
    _timers[timer].pscl  = p;
    _timers[timer].t1    = 0;
    _timers[timer].t2    = 0;
    _timers[timer].th    = 0;
    _timers[timer].ovf   = 0;
    _timers[timer].cpt   = 0;

    tim16_ll_reset(_timers[timer].tim);
    tim16_ll_select_input_capture_edge(_timers[timer].tim, TIM16_LL_RISING_EDGE);
    tim16_ll_enable_interrupts(_timers[timer].timsk);

    return TIM16_OK;
}

int tim16_start(tim16_t timer)
{
    if (timer >= NUMBER_OF_TIM16)
        return -TIM16_ERROR_HANDLE;

    if (_timers[timer].state == TIM16_RESET)
        return -TIM16_ERROR_STATE;

    tim16_ll_set_prescaler(_timers[timer].tim, _timers[timer].pscl);
    _timers[timer].state = TIM16_RUNNING;

    return TIM16_OK;

}

int tim16_stop(tim16_t timer)
{
    if (timer >= NUMBER_OF_TIM16)
        return -TIM16_ERROR_HANDLE;

    if (_timers[timer].state == TIM16_RESET)
        return -TIM16_ERROR_STATE;

    tim16_ll_set_prescaler(_timers[timer].tim, TIM16_LL_PSCL_NONE);
    _timers[timer].state = TIM16_STOPPED;

    return TIM16_OK;
}

int tim16_capture_input(tim16_t timer, unsigned int * period, uint8_t * duty_cycle)
{
    static const uint8_t _factors[NUMBER_OF_TIM16_PRESCALERS] = {
        [TIM16_PRESCALER_1]    = 0,  // 2^0
        [TIM16_PRESCALER_8]    = 3,  // 2^3
        [TIM16_PRESCALER_64]   = 6,  // 2^6
        [TIM16_PRESCALER_256]  = 8,  // 2^8
        [TIM16_PRESCALER_1024] = 10, // 2^10
    };

    if (timer >= NUMBER_OF_TIM16)
        return -TIM16_ERROR_HANDLE;

    if (_timers[timer].state != TIM16_RUNNING)
        return -TIM16_ERROR_STATE;

    if (_timers[timer].ovf > 2)
        return -TIM16_ERROR_OVERFLOW;

    if (_timers[timer].cpt < 3)
        return -TIM16_ERROR_NO_DATA;

    unsigned long f_timer = F_CPU >> _factors[_timers[timer].pscl];
    uint16_t ticks = _timers[timer].t2 - _timers[timer].t1;
    uint16_t high = _timers[timer].th - _timers[timer].t1;

    *period = f_timer/ticks;
    *duty_cycle = (255u*high)/ticks;

    return TIM16_OK;
}
