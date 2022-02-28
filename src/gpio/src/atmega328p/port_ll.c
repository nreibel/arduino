#include "types.h"
#include "port_ll.h"
#include "gpio_ll.h"

#include <avr/interrupt.h>

/* Interrupt handlers */

ISR(INT0_vect)
{
    gpio_extint_cbk(EXTINT0);
}

ISR(INT1_vect)
{
    gpio_extint_cbk(EXTINT1);
}

ISR(PCINT0_vect)
{
    gpio_pcint_cbk(PCINTB, PORTS[PORT_B].pin);
}

ISR(PCINT1_vect)
{
    gpio_pcint_cbk(PCINTC, PORTS[PORT_C].pin);
}

ISR(PCINT2_vect)
{
    gpio_pcint_cbk(PCINTD, PORTS[PORT_D].pin);
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

int gpio_ll_set_edge(extint_t pin, gpio_edge_t edge)
{
    switch(pin)
    {
        case EXTINT0: eicra->bits.isc0 = edge; break;
        case EXTINT1: eicra->bits.isc1 = edge; break;
        default: return -GPIO_LL_ERROR_PIN;
    }

    return GPIO_LL_OK;
}

int gpio_ll_enable_extint(extint_t pin)
{
    switch(pin)
    {
        case EXTINT0: eimsk->bits.int0 = 1; break;
        case EXTINT1: eimsk->bits.int1 = 1; break;
        default: return -GPIO_LL_ERROR_PIN;
    }

    return GPIO_LL_OK;
}

int gpio_ll_disable_extint(extint_t pin)
{
    switch(pin)
    {
        case EXTINT0: eimsk->bits.int0 = 0; break;
        case EXTINT1: eimsk->bits.int1 = 0; break;
        default: return -GPIO_LL_ERROR_PIN;
    }

    return GPIO_LL_OK;
}