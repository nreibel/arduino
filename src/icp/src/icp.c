#include "icp.h"
#include "os.h"
#include "bits.h"
#include "avr/io.h"
#include "avr/interrupt.h"

static struct {
    volatile bool ovf;    // Timer has overflown
    volatile uint16_t th; // High duration
    volatile uint16_t tl; // Low duration
} cfg[NUMBER_OF_ICP] = {
    [ICP1] = { FALSE, 0, 0 }
};

ISR(TIMER1_CAPT_vect)
{
    if ( IS_SET_BIT(TCCR1B, ICES1) ) cfg[ICP1].tl = ICR1;
    else cfg[ICP1].th = ICR1;
    TOGGLE_BIT(TCCR1B, ICES1);
    TCNT1 = 0;
}

ISR(TIMER1_OVF_vect)
{
    cfg[ICP1].ovf = TRUE;
}

int icp_init(icp_t self, icp_prescaler_t prescaler)
{
    uint8_t pscl = 0x0;

    switch(prescaler)
    {
        case ICP_PRESCALER_1:    pscl = 0x1; break;
        case ICP_PRESCALER_8:    pscl = 0x2; break;
        case ICP_PRESCALER_64:   pscl = 0x3; break;
        case ICP_PRESCALER_256:  pscl = 0x4; break;
        case ICP_PRESCALER_1024: pscl = 0x5; break;
        default: return -1;
    }

    switch(self)
    {
        case ICP1:
            os_interrupts_disable();
            RESET_BIT(PRR, PRTIM1);              // Enable Timer 1
            RESET_BIT(DDRB, 0);                  // ICR1 is on PB0
            TCCR1A = 0x0;                        // Normal port operation
            TCCR1B = BIT(ICES1)|MASK(pscl, 0x7); // Capture rising edge, set prescaler
            TIMSK1 = BIT(ICIE1)|BIT(TOIE1);      // Enable Input Capture Interrupt and Timer Overflow Interrupt
            TCNT1 = 0;
            ICR1 = 0;
            os_interrupts_enable();
            break;

        default:
            return -1;
    }

    return 0;
}

int icp_get_duty_cycle(icp_t self, float * duty_cycle)
{
    switch(self)
    {
        case ICP1:
        {
            if (cfg[self].th == 0)
            {
                *duty_cycle = 0.0;
            }
            if (cfg[self].tl == 0)
            {
                *duty_cycle = 1.0;
            }
            else
            {
                float period = cfg[self].th + cfg[self].tl;
                *duty_cycle = cfg[self].th/period;
            }

            return 0;
        }

        default:
            return -1;
    }

    return 0;
}

int icp_is_overflow(icp_t self, bool * overflow)
{
    switch(self)
    {
        case ICP1:
            *overflow = cfg[self].ovf;
            return 0;

        default:
            return -1;
    }

    return 0;
}
