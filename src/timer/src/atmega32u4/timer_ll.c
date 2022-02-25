#include "bits.h"
#include "timer.h"
#include "timer_ll.h"

#include "avr/power.h"

// Timer object declaration
timer_handle_t TIMER[NUMBER_OF_TIMERS] = {
    [TIMER_0] = {
        .init = FALSE,
        .instance = TIM0,
        .prescaler = TIMER_PRESCALER_STOPPED,
        .oca = { .port = PORT_B, .pin = 7 },
        .ocb = { .port = PORT_D, .pin = 0 }
    }
};

int timer_ll_power_enable(timer_t timer)
{
    switch(timer)
    {
        case TIMER_0: power_timer0_enable(); break;
        default: return -TIMER_LL_ERROR_INSTANCE;
    }

    return TIMER_LL_OK;
}

int timer_ll_set_imask(timer_t timer, uint8_t imask)
{
    switch(timer)
    {
        case TIMER_0: TIMSK0 = MASK(imask, 0x7); break;
        default: return -TIMER_LL_ERROR_INSTANCE;
    }

    return TIMER_LL_OK;
}

int timer_ll_set_prescaler(timer_t timer, uint8_t prescaler)
{
    uint8_t reg = 0;

    switch(timer)
    {
        case TIMER_0:
        {
            switch(prescaler)
            {
                case TIMER_PRESCALER_STOPPED: reg = 0x0; break;
                case TIMER_PRESCALER_1:       reg = 0x1; break;
                case TIMER_PRESCALER_8:       reg = 0x2; break;
                case TIMER_PRESCALER_64:      reg = 0x3; break;
                case TIMER_PRESCALER_256:     reg = 0x4; break;
                case TIMER_PRESCALER_1024:    reg = 0x5; break;
                default: return -TIMER_LL_ERROR_PRESCALER;
            }

            break;
        }

        default:
            return -TIMER_LL_ERROR_INSTANCE;
    }

    TIMER[timer].instance->TCCRB.bits.CS = reg;
    return TIMER_LL_OK;
}
