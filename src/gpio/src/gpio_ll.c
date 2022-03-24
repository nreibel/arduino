#include "os.h"
#include "os_mem.h"
#include "os_cfg.h"
#include "gpio_ll.h"
#include "bits.h"
#include "types.h"

#include <avr/io.h>

/*
 * Interrupt handlers
 */

__attribute((weak))
void gpio_extint_cbk(extint_t i)
{
    UNUSED(i);
}

__attribute((weak))
void gpio_pcint_cbk(pcint_t i, uint8_t val)
{
    UNUSED(i);
    UNUSED(val);
}

/*
 * Exported functions
 */

void gpio_ll_enable_pcint(pcint_t port, uint8_t mask)
{
    PCMSK[port] = mask;
    SET_BIT(PCICR, port);
}

void gpio_ll_disable_pcint(pcint_t port)
{
    RESET_BIT(PCICR, port);
    PCMSK[port] = 0x0;
}

void gpio_ll_set_port_data_direction(port_t port, uint8_t mask)
{
    PORTS[port].ddr = mask;
}

uint8_t gpio_ll_get_port_value(port_t port)
{
    return PORTS[port].pin;
}

void gpio_ll_set_port_value(port_t port, uint8_t mask)
{
    PORTS[port].port = mask;
}

void gpio_ll_toggle_port(port_t port, uint8_t mask)
{
    PORTS[port].ddr = mask;
}

void gpio_ll_set_data_direction(port_t port, uint8_t pin, bool output)
{
    if(output) SET_BIT(PORTS[port].ddr, pin);
    else RESET_BIT(PORTS[port].ddr, pin);
}

bool gpio_ll_get(port_t port, uint8_t pin)
{
    return IS_SET_BIT(PORTS[port].pin, pin) ? TRUE : FALSE;
}

void gpio_ll_set(port_t port, uint8_t pin)
{
    SET_BIT(PORTS[port].port, pin);
}

void gpio_ll_toggle(port_t port, uint8_t pin)
{
    SET_BIT(PORTS[port].pin, pin);
}

void gpio_ll_reset(port_t port, uint8_t pin)
{
    RESET_BIT(PORTS[port].port, pin);
}
