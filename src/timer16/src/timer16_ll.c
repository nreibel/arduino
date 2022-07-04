#include "timer16_ll.h"
#include "types.h"

// static const mem_timer16_t timer[NUMBER_OF_TIMER16] = {
//     [TIMER1] = (void*) 0x80,
//     [TIMER3] = (void*) 0x90,
// };
//
// static const timer16_tifr_t tifr[NUMBER_OF_TIMER16] = {
//     [TIMER1] = (void*) 0x36,
//     [TIMER3] = (void*) 0x38,
// };

int tim16_ll_set_prescaler(mem_tim16_t tim, tim16_prescaler_t pscl)
{
    tim->tccrb.b.cs = pscl;
    return TIMER16_LL_OK;
}

int tim16_ll_select_input_capture_edge(mem_tim16_t tim, tim16_edge_t edge)
{
    switch(edge)
    {
        case TIM16_LL_RISING_EDGE:
            tim->tccrb.b.ices = 1;
            break;

        case TIM16_LL_FALLING_EDGE:
            tim->tccrb.b.ices = 0;
            break;

        default:
            return -TIMER16_LL_ERROR_EDGE;
    }

    return TIMER16_LL_OK;
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

bool tim16_ll_check_ovf(tim16_tifr_t tifr)
{
    return tifr->b.tov == 1 ? true : false;
}

bool tim16_ll_check_icf(tim16_tifr_t tifr)
{
    return tifr->b.icf == 1 ? true : false;
}

void tim16_ll_reset_ovf(tim16_tifr_t tifr)
{
    tifr->b.tov = 1;
}

void tim16_ll_reset_icf(tim16_tifr_t tifr)
{
    tifr->b.icf = 1;
}
