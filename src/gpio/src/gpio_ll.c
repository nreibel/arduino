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

#if GPIO_EXTINT
__attribute((weak))
void gpio_extint_cbk(extint_t i)
{
    UNUSED(i);
}
#endif // GPIO_EXTINT

#if GPIO_PCINT
__attribute((weak))
void gpio_pcint_cbk(pcint_t i, uint8_t val)
{
    UNUSED(i);
    UNUSED(val);
}
#endif // GPIO_PCINT

/*
 * Exported functions
 */

#if GPIO_PCINT
int gpio_ll_enable_pcint(pcint_t port, uint8_t mask)
{
    if (port >= NUMBER_OF_PCINTS)
        return -GPIO_LL_ERROR_PORT;

    PCMSK[port] = mask;
    SET_BIT(PCICR, port);

    return GPIO_LL_OK;
}

int gpio_ll_disable_pcint(pcint_t port)
{
    if (port >= NUMBER_OF_PCINTS)
        return -GPIO_LL_ERROR_PORT;

    RESET_BIT(PCICR, port);
    PCMSK[port] = 0x0;

    return GPIO_LL_OK;
}
#endif // GPIO_PCINT

int gpio_ll_set_port_data_direction(port_t port, uint8_t mask)
{
    if (port >= NUMBER_OF_PORTS)
        return -GPIO_LL_ERROR_PORT;

    PORTS[port].ddr = mask;

    return GPIO_LL_OK;
}

int gpio_ll_get_port_value(port_t port, uint8_t *mask)
{
    if (port >= NUMBER_OF_PORTS)
        return -GPIO_LL_ERROR_PORT;

    *mask= PORTS[port].pin;

    return GPIO_LL_OK;
}

int gpio_ll_set_port_value(port_t port, uint8_t mask)
{
    if (port >= NUMBER_OF_PORTS)
        return -GPIO_LL_ERROR_PORT;

    PORTS[port].port = mask;

    return GPIO_LL_OK;
}

int gpio_ll_toggle_port(port_t port, uint8_t mask)
{
    if (port >= NUMBER_OF_PORTS)
        return -GPIO_LL_ERROR_PORT;

    PORTS[port].ddr = mask;

    return GPIO_LL_OK;
}

int gpio_ll_set_data_direction(port_t port, uint8_t pin, bool output)
{
    if (port >= NUMBER_OF_PORTS)
        return -GPIO_LL_ERROR_PORT;

    if (pin >= 8)
        return -GPIO_LL_ERROR_PIN;

    if(output) SET_BIT(PORTS[port].ddr, pin);
    else RESET_BIT(PORTS[port].ddr, pin);

    return GPIO_LL_OK;
}

int gpio_ll_get(port_t port, uint8_t pin, bool *state)
{
    if (port >= NUMBER_OF_PORTS)
        return -GPIO_LL_ERROR_PORT;

    if (pin >= 8)
        return -GPIO_LL_ERROR_PIN;

    *state = IS_SET_BIT(PORTS[port].pin, pin) ? TRUE : FALSE;

    return GPIO_LL_OK;
}

int gpio_ll_set(port_t port, uint8_t pin)
{
    if (port >= NUMBER_OF_PORTS)
        return -GPIO_LL_ERROR_PORT;

    if (pin >= 8)
        return -GPIO_LL_ERROR_PIN;

    SET_BIT(PORTS[port].port, pin);

    return GPIO_LL_OK;
}

int gpio_ll_toggle(port_t port, uint8_t pin)
{
    if (port >= NUMBER_OF_PORTS)
        return -GPIO_LL_ERROR_PORT;

    if (pin >= 8)
        return -GPIO_LL_ERROR_PIN;

    SET_BIT(PORTS[port].pin, pin);

    return GPIO_LL_OK;
}

int gpio_ll_reset(port_t port, uint8_t pin)
{
    if (port >= NUMBER_OF_PORTS)
        return -GPIO_LL_ERROR_PORT;

    if (pin >= 8)
        return -GPIO_LL_ERROR_PIN;

    RESET_BIT(PORTS[port].port, pin);

    return GPIO_LL_OK;
}
