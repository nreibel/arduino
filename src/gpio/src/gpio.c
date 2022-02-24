#include "gpio.h"
#include "bits.h"
#include "types.h"
#include "os_cfg.h"

#include <avr/io.h>
#include <util/delay.h>

/* Private data */

static gpio_extint_cbk_t extint_cbk[NUMBER_OF_EXTINTS] = {0};
static volatile void* extint_data[NUMBER_OF_EXTINTS] = {0};

static gpio_pcint_cbk_t pcint_cbk[NUMBER_OF_PCINTS] = {0};
static volatile void* pcint_data[NUMBER_OF_PCINTS] = {0};

/* LowLevel API */

extern void gpio_ll_set_edge(extint_t pin, uint8_t edge);
extern void gpio_ll_enable_extint(extint_t pin);
extern void gpio_ll_disable_extint(extint_t pin);
static void gpio_ll_enable_pcint(pcint_t port, uint8_t mask);
static void gpio_ll_disable_pcint(pcint_t port);

/* Interrupt routines */

void gpio_extint(extint_t i)
{
    if (i < NUMBER_OF_EXTINTS && extint_cbk[i] != NULL_PTR)
    {
        (*extint_cbk[i])(i, extint_data[i]);
    }
}

void gpio_pcint(pcint_t i, uint8_t val)
{
    if (i < NUMBER_OF_PCINTS && pcint_cbk[i] != NULL_PTR)
    {
        (*pcint_cbk[i])(i, val, pcint_data[i]);
    }
}

/* External interrupts */

int gpio_enable_pcint(pcint_t port, uint8_t mask, gpio_pcint_cbk_t cbk, volatile void *data)
{
    if (port >= NUMBER_OF_PCINTS)
        return -GPIO_INVALID_PORT;

    gpio_ll_enable_pcint(port, mask);

    pcint_cbk[port] = cbk;
    pcint_data[port] = data;

    return GPIO_OK;
}

int gpio_disable_pcint(pcint_t port)
{
    if (port >= NUMBER_OF_PCINTS)
        return -GPIO_INVALID_PORT;

    gpio_ll_disable_pcint(port);

    pcint_cbk[port] = NULL_PTR;
    pcint_data[port] = NULL_PTR;

    return GPIO_OK;
}

int gpio_enable_extint(extint_t pin, gpio_edge_t edge, gpio_extint_cbk_t cbk, volatile void *data)
{
    if (pin >= NUMBER_OF_EXTINTS)
        return -GPIO_INVALID_INT;

    switch(edge)
    {
        case GPIO_EDGE_LOW:
        case GPIO_EDGE_ANY:
        case GPIO_EDGE_FALLING:
        case GPIO_EDGE_RISING:
            gpio_ll_set_edge(pin, edge);
            gpio_ll_enable_extint(pin);
            break;

        default:
            return -GPIO_INVALID_MODE;
    }

    extint_data[pin] = data;
    extint_cbk[pin] = cbk;

    return GPIO_OK;
}

int gpio_disable_extint(extint_t pin)
{
    if (pin >= NUMBER_OF_EXTINTS)
        return -GPIO_INVALID_INT;

    gpio_ll_disable_extint(pin);

    extint_data[pin] = NULL_PTR;
    extint_cbk[pin] = NULL_PTR;

    return GPIO_OK;
}

/* Public functions */

int gpio_init(gpio_t self, port_t port, uint8_t pin, gpio_data_direction_t direction)
{
    if (pin >= 8)
        return -GPIO_INVALID_PIN;

    if (port >= NUMBER_OF_PORTS)
        return -GPIO_INVALID_PORT;

    self->port = port;
    self->pin = pin;

    return gpio_set_data_direction(self, direction);
}

int gpio_set_data_direction(gpio_t self, gpio_data_direction_t direction)
{
    // Set data direction
    switch(direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
        case GPIO_OUTPUT_ACTIVE_LOW:
            SET_BIT(PORTS[self->port].DDR, self->pin);
            break;

        case GPIO_INPUT_HIGH_Z:
        case GPIO_INPUT_PULLUP:
            RESET_BIT(PORTS[self->port].DDR, self->pin);
            break;

        default:
            return -GPIO_INVALID_MODE;
    }

    // Set output state or pullup
    switch(direction)
    {
        case GPIO_OUTPUT_ACTIVE_LOW:
        case GPIO_INPUT_PULLUP:
            SET_BIT(PORTS[self->port].PORT, self->pin);
            break;

        case GPIO_OUTPUT_ACTIVE_HIGH:
        case GPIO_INPUT_HIGH_Z:
            RESET_BIT(PORTS[self->port].PORT, self->pin);
            break;

        default:
            return -GPIO_INVALID_MODE;
    }

    self->direction = direction;

    return GPIO_OK;
}

int gpio_get(gpio_t self, bool *state)
{
    switch(self->direction)
    {
        case GPIO_INPUT_HIGH_Z:
        case GPIO_INPUT_PULLUP:
            *state = IS_SET_BIT(PORTS[self->port].PIN, self->pin) ? TRUE : FALSE;
            return GPIO_OK;

        default:
            return -GPIO_INVALID_MODE;
    }
}

int gpio_set(gpio_t self)
{
    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
            SET_BIT(PORTS[self->port].PORT, self->pin);
            return GPIO_OK;

        case GPIO_OUTPUT_ACTIVE_LOW:
            RESET_BIT(PORTS[self->port].PORT, self->pin);
            return GPIO_OK;

        default:
            return -GPIO_INVALID_MODE;
    }
}

int gpio_toggle(gpio_t self)
{
    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
        case GPIO_OUTPUT_ACTIVE_LOW:
            SET_BIT(PORTS[self->port].PIN, self->pin);
            return GPIO_OK;

        default:
            return -GPIO_INVALID_MODE;
    }
}

int gpio_reset(gpio_t self)
{
    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
            RESET_BIT(PORTS[self->port].PORT, self->pin);
            return GPIO_OK;

        case GPIO_OUTPUT_ACTIVE_LOW:
            SET_BIT(PORTS[self->port].PORT, self->pin);
            return GPIO_OK;

        default:
            return -GPIO_INVALID_MODE;
    }
}

/* Private functions */

static void gpio_ll_enable_pcint(pcint_t port, uint8_t mask)
{
    PCMSK[port] = mask;
    SET_BIT(PCICR, port);
}

static void gpio_ll_disable_pcint(pcint_t port)
{
    RESET_BIT(PCICR, port);
    PCMSK[port] = 0;
}
