#include "icp.h"
#include "os.h"
#include "bits.h"
#include "gpio_ll.h"

#include "avr/io.h"
#include "avr/power.h"
#include "avr/interrupt.h"

/*
 * Interrupt routines
 */

// TODO
//
// __attribute__((weak))
// void icp_overflow(icp_t icp)
// {
//     UNUSED(icp);
// }
//
// ISR(TIMER1_CAPT_vect)
// {
//     static volatile uint16_t period = 0;
//     static volatile uint16_t t_high = 0;
//
//     if (ICP1->tccrb.bits.ices)
//     {
//         // Rising edge
//         ICP1->tcnt = 0;
//         period = ICP1->icr;
//     }
//     else
//     {
//         // Falling edge
//         t_high = ICP1->icr;
//     }
//
//     ICP1->tccrb.bits.ices = ICP1->tccrb.bits.ices ? 0 : 1;
// }
//
// ISR(TIMER1_OVF_vect)
// {
//     icp_overflow(ICP1);
// }

/*
 * Public functions
 */

int icp_init(icp_handle_t self, icp_t icp, icp_prescaler_t prescaler)
{
    if (self == NULL_PTR)
        return -ICP_ERROR_INSTANCE;

    if (icp != ICP1)
        return -ICP_ERROR_INSTANCE;

    switch(prescaler)
    {
        case ICP_PRESCALER_1:    self->factor = 0;  break; // 2^0
        case ICP_PRESCALER_8:    self->factor = 3;  break; // 2^3
        case ICP_PRESCALER_64:   self->factor = 6;  break; // 2^6
        case ICP_PRESCALER_256:  self->factor = 8;  break; // 2^8
        case ICP_PRESCALER_1024: self->factor = 10; break; // 2^10
        default: return -ICP_ERROR_PRESCALER;
    }

    // Enable Timer 1
    power_timer1_enable();

    // Set ICR1 (PB0) as input
    gpio_ll_set_data_direction(PORT_B, 0, false);

    // Reset device
    icp_ll_init(icp);

    self->prescaler = prescaler;
    self->instance = icp;

    return ICP_OK;
}

int icp_get_frequency(icp_handle_t self, uint16_t * frequency)
{
    icp_ll_reset(self->instance);
    icp_ll_clear_ovf(self->instance);
    icp_ll_set_prescaler(self->instance, self->prescaler);
    uint16_t t1 = icp_ll_capture_edge(self->instance, true);
    uint16_t t2 = icp_ll_capture_edge(self->instance, true);
    icp_ll_set_prescaler(self->instance, ICP_PRESCALER_NONE);

    if ( icp_ll_is_ovf(self->instance) ) return -ICP_ERROR_OVERFLOW;

    uint16_t period = t2 - t1;
    *frequency = (F_CPU >> self->factor) / period;

    return ICP_OK;
}

int icp_get_duty_cycle(icp_handle_t self, uint8_t * duty_cycle)
{
    icp_ll_reset(self->instance);
    icp_ll_clear_ovf(self->instance);
    icp_ll_set_prescaler(self->instance, self->prescaler);
    uint16_t t1 = icp_ll_capture_edge(self->instance, true);
    uint16_t t2 = icp_ll_capture_edge(self->instance, false);
    uint16_t t3 = icp_ll_capture_edge(self->instance, true);
    icp_ll_set_prescaler(self->instance, ICP_PRESCALER_NONE);

    if ( icp_ll_is_ovf(self->instance) ) return -ICP_ERROR_OVERFLOW;

    uint16_t t_high = t2 - t1;
    uint16_t period = t3 - t1;

    *duty_cycle = 255UL * t_high / period;

    return ICP_OK;
}

/*
 * Private functions
 */

void icp_ll_init(icp_t icp)
{
    icp->tccra.reg = 0x0;
    icp->tccrb.reg = 0x0;
    icp->tccrc.reg = 0x0;
}

void icp_ll_reset(icp_t icp)
{
    icp->tcnt = 0;
}

void icp_ll_set_prescaler(icp_t icp, uint8_t prescaler)
{
    icp->tccrb.bits.cs = prescaler;
}

uint16_t icp_ll_capture_edge(icp_t icp, bool rising)
{
    icp->tccrb.bits.ices = rising ? 1 : 0;

    // Clear interrupt
    SET_BIT(TIFR1, ICF1);

    // Wait for next interrupt
    while (!IS_SET_BIT(TIFR1, ICF1));

    return icp->icr;
}

bool icp_ll_is_ovf(icp_t icp)
{
    UNUSED(icp);
    return IS_SET_BIT(TIFR1, TOV1);
}

void icp_ll_clear_ovf(icp_t icp)
{
    UNUSED(icp);
    SET_BIT(TIFR1, TOV1);
}

void icp_ll_set_interrupt_enabled(icp_t icp, bool enabled)
{
    UNUSED(icp);

    timsk1_t timsk = {
        .bits = {
            .icie = enabled ? 1 : 0,
            .toie = enabled ? 1 : 0,
        }
    };

    TIMSK1 = timsk.reg;
}
