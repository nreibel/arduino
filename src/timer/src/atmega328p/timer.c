#include "bits.h"
#include "timer_ll.h"
#include "gpio_ll.h"

#include <avr/power.h>

/*
 * Static allocation
 */

const mem_timer_t __timers[NUMBER_OF_TIMERS] = {
    [TIMER0] = TIM0,
    [TIMER2] = TIM2,
};

/*
 * Exported functions
 */

int timer_ll_power_enable(timer_t timer)
{
    switch(timer)
    {
        case TIMER0: power_timer0_enable(); break;
        case TIMER2: power_timer2_enable(); break;
        default: return -TIMER_LL_ERROR_INSTANCE;
    }

    return TIMER_LL_OK;
}

int timer_ll_set_imask(timer_t timer, uint8_t imask)
{
    switch(timer)
    {
        case TIMER0: TIMSK0 = MASK(imask, 0x7); break;
        case TIMER2: TIMSK2 = MASK(imask, 0x7); break;
        default: return -TIMER_LL_ERROR_INSTANCE;
    }

    return TIMER_LL_OK;
}

int timer_ll_enable_oca(timer_t timer)
{
    switch(timer)
    {
        case TIMER0: gpio_ll_set_data_direction(PORT_D, 6, TRUE); break;
        case TIMER2: gpio_ll_set_data_direction(PORT_B, 3, TRUE); break;
        default: return -TIMER_LL_ERROR_INSTANCE;
    }

    return TIMER_LL_OK;
}

int timer_ll_enable_ocb(timer_t timer)
{
    switch(timer)
    {
        case TIMER0: gpio_ll_set_data_direction(PORT_D, 5, TRUE); break;
        case TIMER2: gpio_ll_set_data_direction(PORT_D, 3, TRUE); break;
        default: return -TIMER_LL_ERROR_INSTANCE;
    }

    return TIMER_LL_OK;
}
