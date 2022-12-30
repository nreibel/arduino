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

ISR(INT2_vect)
{
    gpio_extint_cbk(EXTINT2);
}

ISR(INT3_vect)
{
    gpio_extint_cbk(EXTINT3);
}

ISR(INT6_vect)
{
    gpio_extint_cbk(EXTINT6);
}

ISR(PCINT0_vect)
{
    gpio_pcint_cbk(PCINTB, PORTS[PORT_B].pin);
}
