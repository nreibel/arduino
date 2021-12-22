#include "icp.h"
#include "os.h"
#include "bits.h"
#include "avr/io.h"
#include "avr/interrupt.h"

static struct {
    volatile bool ovf;    // Timer has overflown
    volatile uint16_t th; // High duration
    volatile uint16_t tl; // Low duration
    uint8_t factor;       // Prescaler factor
} cfg[NUMBER_OF_ICP] = {
    [ICP1] = { FALSE, 0, 0, 0 }
};

ISR(TIMER1_CAPT_vect)
{
    TCNT1 = 0;
    if (IS_SET_BIT(TCCR1B, ICES1)) cfg[ICP1].tl = ICR1;
    else cfg[ICP1].th = ICR1;
    TOGGLE_BIT(TCCR1B, ICES1);
    cfg[ICP1].ovf = FALSE;
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
        case ICP_PRESCALER_1: // 2^0
            cfg[self].factor = 0;
            pscl = 0x1;
            break;

        case ICP_PRESCALER_8: // 2^3
            cfg[self].factor = 3;
            pscl = 0x2;
            break;

        case ICP_PRESCALER_64: // 2^6
            cfg[self].factor = 6;
            pscl = 0x3;
            break;

        case ICP_PRESCALER_256: // 2^8
            cfg[self].factor = 8;
            pscl = 0x4;
            break;

        case ICP_PRESCALER_1024: // 2^10
            cfg[self].factor = 10;
            pscl = 0x5;
            break;

        default:
            return -ICP_ERROR_PARAM;
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
            return -ICP_ERROR_INSTANCE;
    }

    return ICP_OK;
}

int icp_get_frequency(icp_t self, uint16_t * frequency)
{
    switch(self)
    {
        case ICP1:
        {
            if (cfg[self].ovf)
                return -ICP_ERROR_OVERFLOW;

            *frequency = (F_CPU >> cfg[self].factor)/(cfg[self].tl + cfg[self].th);
            break;
        }

        default:
            return -ICP_ERROR_INSTANCE;
    }
    return ICP_OK;
}

int icp_get_duty_cycle(icp_t self, float * duty_cycle)
{
    switch(self)
    {
        case ICP1:
        {
            if (cfg[self].ovf)
                return -ICP_ERROR_OVERFLOW;

            if (cfg[self].th == 0)
            {
                *duty_cycle = 0.0;
            }
            else if (cfg[self].tl == 0)
            {
                *duty_cycle = 1.0;
            }
            else
            {
                float period = cfg[self].th + cfg[self].tl;
                float dc = cfg[self].th/period;
                *duty_cycle = LIMIT(dc, 0.0, 1.0);
            }
            break;
        }

        default:
            return -ICP_ERROR_INSTANCE;
    }

    return ICP_OK;
}
