#include <avr/interrupt.h>

#include "tim16.h"
#include "tim16_ll.h"
#include "types.h"

ISR(TIMER1_CAPT_vect)
{
    if (_TIM1->tccrb.b.ices)
    {
        tim16_ll_icp_cbk(_TIM1, TIM16_LL_RISING_EDGE, _TIM1->icr);
        _TIM1->tccrb.b.ices = 0;
    }
    else
    {
        tim16_ll_icp_cbk(_TIM1, TIM16_LL_FALLING_EDGE, _TIM1->icr);
        _TIM1->tccrb.b.ices = 1;
    }
}

ISR(TIMER3_CAPT_vect)
{
    if (_TIM3->tccrb.b.ices)
    {
        tim16_ll_icp_cbk(_TIM3, TIM16_LL_RISING_EDGE, _TIM3->icr);
        _TIM3->tccrb.b.ices = 0;
    }
    else
    {
        tim16_ll_icp_cbk(_TIM3, TIM16_LL_FALLING_EDGE, _TIM3->icr);
        _TIM3->tccrb.b.ices = 1;
    }
}

ISR(TIMER1_OVF_vect)
{
    tim16_ll_ovf_cbk(_TIM1);
}

ISR(TIMER3_OVF_vect)
{
    tim16_ll_ovf_cbk(_TIM3);
}

__attribute__((weak))
void tim16_ll_icp_cbk(mem_tim16_t timer, tim16_edge_t edge, uint16_t value)
{
    UNUSED(timer);
    UNUSED(edge);
    UNUSED(value);
}

__attribute__((weak))
void tim16_ll_ovf_cbk(mem_tim16_t timer)
{
    UNUSED(timer);
}

void tim16_ll_set_prescaler(mem_tim16_t tim, tim16_ll_pscl_t pscl)
{
    tim->tccrb.b.cs = pscl;
}

void tim16_ll_select_input_capture_edge(mem_tim16_t tim, tim16_edge_t edge)
{
    tim->tccrb.b.ices = edge;
}

void tim16_ll_enable_interrupts(tim16_timsk_t timsk)
{
    timsk->b.icie = 1;
    timsk->b.toie = 1;
}

void tim16_ll_reset_value(mem_tim16_t tim)
{
    tim->tcnt = 0;
}

uint16_t tim16_ll_get_value(mem_tim16_t tim)
{
    return tim->tcnt;
}

uint16_t tim16_ll_get_input_capture(mem_tim16_t tim)
{
    return tim->icr;
}
