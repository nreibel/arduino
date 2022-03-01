#include "timer_ll.h"
#include "types.h"

extern const mem_timer_t __timers[NUMBER_OF_TIMERS];

int timer_ll_set_prescaler(timer_t timer, uint8_t pscl)
{
    __timers[timer]->tccrb.bits.cs = pscl;
    return TIMER_LL_OK;
}

int timer_ll_set_wgm(timer_t timer, uint8_t wgm)
{
    __timers[timer]->tccra.bits.wgm = wgm;
    return TIMER_LL_OK;
}

int timer_ll_set_ocra(timer_t timer, uint8_t val)
{
    __timers[timer]->ocra = val;
    return TIMER_LL_OK;
}

int timer_ll_set_ocrb(timer_t timer, uint8_t val)
{
    __timers[timer]->ocrb = val;
    return TIMER_LL_OK;
}

int timer_ll_set_oca_mode(timer_t timer, bool top)
{
    __timers[timer]->tccrb.bits.wgm = top ? 1 : 0;
    return TIMER_LL_OK;
}

int timer_ll_reset(timer_t timer)
{
    __timers[timer]->tccra.reg = 0x0;
    __timers[timer]->tccrb.reg = 0x0;
    return TIMER_LL_OK;
}

int timer_ll_set_coma(timer_t timer, uint8_t val)
{
    __timers[timer]->tccra.bits.coma = val;
    return TIMER_LL_OK;
}

int timer_ll_set_comb(timer_t timer, uint8_t val)
{
    __timers[timer]->tccra.bits.comb = val;
    return TIMER_LL_OK;
}
