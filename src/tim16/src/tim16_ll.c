#include <avr/interrupt.h>

#include "tim16.h"
#include "tim16_ll.h"
#include "types.h"

static void tim16_capt_vect(mem_tim16_t tim);

ISR(TIMER1_CAPT_vect) { tim16_capt_vect(_TIM1);  }
ISR(TIMER3_CAPT_vect) { tim16_capt_vect(_TIM3);  }
ISR(TIMER1_OVF_vect)  { tim16_ll_ovf_cbk(_TIM1); }
ISR(TIMER3_OVF_vect)  { tim16_ll_ovf_cbk(_TIM3); }

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

static void tim16_capt_vect(mem_tim16_t tim)
{
    if (tim->tccrb.b.ices)
    {
        tim16_ll_icp_cbk(tim, TIM16_LL_RISING_EDGE, tim->icr);
        tim->tccrb.b.ices = 0;
    }
    else
    {
        tim16_ll_icp_cbk(tim, TIM16_LL_FALLING_EDGE, tim->icr);
        tim->tccrb.b.ices = 1;
    }
}

void tim16_ll_reset(mem_tim16_t tim)
{
    tim->tccra.raw = 0x0;
    tim->tccrb.raw = 0x0;
    tim->tccrc.raw = 0x0;
    tim->ocra = 0x0;
    tim->ocrb = 0x0;
    tim->ocrc = 0x0;
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

