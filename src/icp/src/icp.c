#include "icp.h"
#include "os.h"
#include "bits.h"
#include "avr/io.h"
#include "avr/interrupt.h"

/*
 * Private data
 */

static struct {
    volatile bool ovf;    // Timer has overflown
    volatile uint16_t th; // High duration
    volatile uint16_t p;  // Period
    uint8_t factor;       // Prescaler factor
    bool interrupts;      // Use interrupts
} cfg[NUMBER_OF_ICP] = {
    [ICP1] = { FALSE, 0, 0, 0, FALSE }
};

/*
 * Private methods declaration
 */

static uint16_t icp_ll_capture_edge(bool rising);
static bool icp_ll_is_ovf();
static void icp_ll_clear_ovf_int();

/*
 * Interrupt routines
 */

ISR(TIMER1_CAPT_vect)
{
    if ( IS_SET_BIT(TCCR1B, ICES1) != FALSE )
    {
        // Rising edge
        TCNT1 = 0;
        cfg[ICP1].p = ICR1;
        cfg[ICP1].ovf = FALSE;
    }
    else
    {
        // Falling edge
        cfg[ICP1].th = ICR1;
    }

    TOGGLE_BIT(TCCR1B, ICES1);
}

ISR(TIMER1_OVF_vect)
{
    cfg[ICP1].ovf = TRUE;
}

/*
 * Public functions
 */

int icp_init(icp_t self, icp_prescaler_t prescaler, bool useInterrupts)
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
            if (useInterrupts)
            {
                cfg[self].interrupts = TRUE;
                TIMSK1 = BIT(ICIE1)|BIT(TOIE1);  // Enable Input Capture Interrupt and Timer Overflow Interrupt
            }
            else
            {
                cfg[self].interrupts = FALSE;
            }
            TCNT1 = 0;
            os_interrupts_enable();
            break;

        default:
            return -ICP_ERROR_INSTANCE;
    }

    return ICP_OK;
}

int icp_get_frequency(icp_t self, uint16_t * frequency)
{
    uint16_t p;

    if (self != ICP1)
        return -ICP_ERROR_INSTANCE;

    if (!cfg[self].interrupts)
    {
        icp_ll_clear_ovf_int();

        uint16_t p1 = icp_ll_capture_edge(TRUE);
        uint16_t p2 = icp_ll_capture_edge(TRUE);

        if (icp_ll_is_ovf() != FALSE)
            return -ICP_ERROR_OVERFLOW;

        p = p2 - p1;
    }
    else
    {
        if (cfg[self].ovf)
            return -ICP_ERROR_OVERFLOW;

        if (cfg[self].p == 0)
            return -ICP_ERROR_NO_DATA;

        p = cfg[self].p;
    }

    *frequency = (F_CPU >> cfg[self].factor)/p;

    return ICP_OK;
}

int icp_get_duty_cycle(icp_t self, uint8_t * duty_cycle)
{
    uint16_t th, p;

    if (self != ICP1)
        return -ICP_ERROR_INSTANCE;

    if (!cfg[self].interrupts)
    {
        icp_ll_clear_ovf_int();

        uint16_t p1 = icp_ll_capture_edge(TRUE);
        uint16_t p2 = icp_ll_capture_edge(FALSE);
        uint16_t p3 = icp_ll_capture_edge(TRUE);

        if (icp_ll_is_ovf() != FALSE)
            return -ICP_ERROR_OVERFLOW;

        th = p2 - p1;
        p = p3 - p1;
    }
    else
    {
        if (cfg[self].ovf)
            return -ICP_ERROR_OVERFLOW;

        if (cfg[self].p == 0)
            return -ICP_ERROR_NO_DATA;

        th = cfg[self].th;
        p = cfg[self].p;
    }

    *duty_cycle = 255UL * th / p;

    return ICP_OK;
}

/*
 * Private functions
 */

static uint16_t icp_ll_capture_edge(bool rising)
{
    if (rising) SET_BIT(TCCR1B, ICES1);
    else RESET_BIT(TCCR1B, ICES1);
    SET_BIT(TIFR1, ICF1);
    while (!IS_SET_BIT(TIFR1, ICF1));
    return ICR1;
}

static bool icp_ll_is_ovf()
{
    return IS_SET_BIT(TIFR1, TOV1);
}

static void icp_ll_clear_ovf_int()
{
    SET_BIT(TIFR1, TOV1);
}
