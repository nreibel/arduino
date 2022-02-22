#include "timer.h"
#include "timer_ll.h"
#include "avr/power.h"

// Timer object declaration
static struct timer_s __timers[NUMBER_OF_TIMERS] = {
    [0] = { .instance = TIM0 }
};

// Timer handles declaration
const timer_t TIMER[NUMBER_OF_TIMERS] = {
    [0] = &__timers[0],
};

void timer_ll_power_enable(ll_timer_t timer)
{
    if (timer == TIM0)
    {
        power_timer0_enable();
    }
}

void timer_ll_set_imask(ll_timer_t timer, uint8_t imask)
{
    if (timer == TIM0)
    {
        TIMSK0 = imask & 0x7;
    }
}

int timer_ll_set_prescaler(ll_timer_t timer, uint8_t prescaler)
{
    if (timer == TIM0)
    {
        switch(prescaler)
        {
            case TIMER_PRESCALER_STOPPED: timer->TCCRB.bits.CS = 0x0; return 0;
            case TIMER_PRESCALER_1:       timer->TCCRB.bits.CS = 0x1; return 0;
            case TIMER_PRESCALER_8:       timer->TCCRB.bits.CS = 0x2; return 0;
            case TIMER_PRESCALER_64:      timer->TCCRB.bits.CS = 0x3; return 0;
            case TIMER_PRESCALER_256:     timer->TCCRB.bits.CS = 0x4; return 0;
            case TIMER_PRESCALER_1024:    timer->TCCRB.bits.CS = 0x5; return 0;
            default: return -1;
        }
    }

    return -1;
}
