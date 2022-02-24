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

ISR(INT2_vect)
{
    gpio_extint(EXTINT_2);
}

ISR(INT3_vect)
{
    gpio_extint(EXTINT_3);
}

ISR(INT6_vect)
{
    gpio_extint(EXTINT_6);
}

ISR(PCINT0_vect)
{
    gpio_pcint(PCINT_B, PORTS[PORT_B].PIN);
}

/* Registers */

static volatile union {
    struct {
        uint8_t isc0 : 2;
        uint8_t isc1 : 2;
        uint8_t isc2 : 2;
        uint8_t isc3 : 2;
    } bits;
    uint8_t reg;
} * eicra = (volatile void *) 0x69;

static volatile union {
    struct {
        uint8_t __reserved1 : 2;
        uint8_t __reserved2 : 2;
        uint8_t isc6        : 2;
        uint8_t __reserved3 : 2;
    } bits;
    uint8_t reg;
} * eicrb = (volatile void *) 0x6A;

static volatile union {
    struct {
        uint8_t int0        : 1;
        uint8_t int1        : 1;
        uint8_t int2        : 1;
        uint8_t int3        : 1;
        uint8_t __reserved1 : 1;
        uint8_t __reserved2 : 1;
        uint8_t int6        : 1;
        uint8_t __reserved3 : 1;
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
        case EXTINT_2: eicra->bits.isc2 = edge; break;
        case EXTINT_3: eicra->bits.isc3 = edge; break;
        case EXTINT_6: eicrb->bits.isc6 = edge; break;
        default: break;
    }
}

void gpio_ll_enable_extint(extint_t pin)
{
    switch(pin)
    {
        case EXTINT_0: eimsk->bits.int0 = 1; break;
        case EXTINT_1: eimsk->bits.int1 = 1; break;
        case EXTINT_2: eimsk->bits.int2 = 1; break;
        case EXTINT_3: eimsk->bits.int3 = 1; break;
        case EXTINT_6: eimsk->bits.int6 = 1; break;
        default: break;
    }
}

void gpio_ll_disable_extint(extint_t pin)
{
    switch(pin)
    {
        case EXTINT_0: eimsk->bits.int0 = 0; break;
        case EXTINT_1: eimsk->bits.int1 = 0; break;
        case EXTINT_2: eimsk->bits.int2 = 0; break;
        case EXTINT_3: eimsk->bits.int3 = 0; break;
        case EXTINT_6: eimsk->bits.int6 = 0; break;
        default: break;
    }
}
