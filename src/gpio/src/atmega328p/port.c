#include "types.h"
#include "port.h"
#include "gpio_ll.h"

#include <avr/interrupt.h>

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
