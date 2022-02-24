#include "types.h"
#include "port_ll.h"

#include <avr/interrupt.h>

extern void gpio_extint(extint_t i);
extern void gpio_pcint(pcint_t i, uint8_t val);

/* Interrupt handlers */

ISR(INT0_vect)
{
    gpio_extint(EXTINT_0);
}

ISR(INT1_vect)
{
    gpio_extint(EXTINT_1);
}

ISR(PCINT0_vect)
{
    gpio_pcint(PCINT_B, PORTS[PORT_B].PIN);
}

ISR(PCINT1_vect)
{
    gpio_pcint(PCINT_C, PORTS[PORT_C].PIN);
}

ISR(PCINT2_vect)
{
    gpio_pcint(PCINT_D, PORTS[PORT_D].PIN);
}

/* Registers */

static volatile union {
    struct {
        uint8_t isc0       : 2;
        uint8_t isc1       : 2;
        uint8_t __reserved : 4;
    } bits;
    uint8_t reg;
} * eicra = (volatile void *) 0x69;

static volatile union {
    struct {
        uint8_t int0       : 1;
        uint8_t int1       : 1;
        uint8_t __reserved : 6;
    } bits;
    uint8_t reg;
} * eimsk = (volatile void *) 0x3D;

/* Exported functions */

void gpio_ll_set_edge(extint_t pin, uint8_t edge)
{
    switch(pin)
    {
        case EXTINT_0: eicra->bits.isc0 = edge; break;
        case EXTINT_1: eicra->bits.isc1 = edge; break;
        default: break;
    }
}

void gpio_ll_enable_extint(extint_t pin)
{
    switch(pin)
    {
        case EXTINT_0: eimsk->bits.int0 = 1; break;
        case EXTINT_1: eimsk->bits.int1 = 1; break;
        default: break;
    }
}

void gpio_ll_disable_extint(extint_t pin)
{
    switch(pin)
    {
        case EXTINT_0: eimsk->bits.int0 = 0; break;
        case EXTINT_1: eimsk->bits.int1 = 0; break;
        default: break;
    }
}
