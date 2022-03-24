#include "gpio.h"
#include "gpio_ll.h"
#include "os_mem.h"
#include "bits.h"
#include "types.h"

/* External interrupts */

int gpio_enable_pcint(pcint_t port, uint8_t mask)
{
    if (port >= NUMBER_OF_PCINTS)
        return -GPIO_ERROR_PORT;

    gpio_ll_enable_pcint(port, mask);

    return GPIO_OK;
}

int gpio_disable_pcint(pcint_t port)
{
    if (port >= NUMBER_OF_PCINTS)
        return -GPIO_ERROR_PORT;

    gpio_ll_disable_pcint(port);

    return GPIO_OK;
}

int gpio_enable_extint(extint_t pin, gpio_edge_t edge)
{
    if (pin >= NUMBER_OF_EXTINTS)
        return -GPIO_ERROR_INT;

    switch(edge)
    {
        case GPIO_EDGE_LOW:
            gpio_ll_set_edge(pin, GPIO_LL_EDGE_LOW);
            break;

        case GPIO_EDGE_ANY:
            gpio_ll_set_edge(pin, GPIO_LL_EDGE_ANY);
            break;

        case GPIO_EDGE_FALLING:
            gpio_ll_set_edge(pin, GPIO_LL_EDGE_FALLING);
            break;

        case GPIO_EDGE_RISING:
            gpio_ll_set_edge(pin, GPIO_LL_EDGE_RISING);
            break;

        default:
            return -GPIO_ERROR_EDGE;
    }

    gpio_ll_enable_extint(pin);

    return GPIO_OK;
}

int gpio_disable_extint(extint_t pin)
{
    if (pin >= NUMBER_OF_EXTINTS)
        return -GPIO_ERROR_INT;

    gpio_ll_disable_extint(pin);

    return GPIO_OK;
}

/* Public functions */

#if OS_MALLOC
gpio_t gpio_create()
{
    gpio_t self = os_malloc(sizeof(*self));
    return self;
}

void gpio_destroy(gpio_t self)
{
    if (self != NULL_PTR)
    {
        os_free(self);
    }
}
#endif // OS_MALLOC

int gpio_init(gpio_t self, port_t port, uint8_t pin)
{
    if (pin >= 8)
        return -GPIO_ERROR_PIN;

    if (port >= NUMBER_OF_PORTS)
        return -GPIO_ERROR_PORT;

    self->port = port;
    self->pin = pin;
    self->direction = GPIO_DIRECTION_NONE;

    return GPIO_OK;
}

int gpio_set_data_direction(gpio_t self, gpio_data_direction_t direction)
{
    // Set data direction
    switch(direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
        case GPIO_OUTPUT_ACTIVE_LOW:
            gpio_ll_set_data_direction(self->port, self->pin, TRUE);
            break;

        case GPIO_INPUT_HIGH_Z:
        case GPIO_INPUT_PULLUP:
            gpio_ll_set_data_direction(self->port, self->pin, FALSE);
            break;

        default:
            return -GPIO_ERROR_DIRECTION;
    }

    // Set output state or pullup
    switch(direction)
    {
        case GPIO_OUTPUT_ACTIVE_LOW:
        case GPIO_INPUT_PULLUP:
            gpio_ll_set(self->port, self->pin);
            break;

        case GPIO_OUTPUT_ACTIVE_HIGH:
        case GPIO_INPUT_HIGH_Z:
            gpio_ll_reset(self->port, self->pin);
            break;

        default:
            return -GPIO_ERROR_DIRECTION;
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
            *state = gpio_ll_get(self->port, self->pin);
            return GPIO_OK;

        default:
            return -GPIO_ERROR_DIRECTION;
    }
}

int gpio_set(gpio_t self)
{
    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
            gpio_ll_set(self->port, self->pin);
            return GPIO_OK;

        case GPIO_OUTPUT_ACTIVE_LOW:
            gpio_ll_reset(self->port, self->pin);
            return GPIO_OK;

        default:
            return -GPIO_ERROR_DIRECTION;
    }
}

int gpio_toggle(gpio_t self)
{
    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
        case GPIO_OUTPUT_ACTIVE_LOW:
            gpio_ll_toggle(self->port, self->pin);
            return GPIO_OK;

        default:
            return -GPIO_ERROR_DIRECTION;
    }
}

int gpio_reset(gpio_t self)
{
    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
            gpio_ll_reset(self->port, self->pin);
            return GPIO_OK;

        case GPIO_OUTPUT_ACTIVE_LOW:
            gpio_ll_set(self->port, self->pin);
            return GPIO_OK;

        default:
            return -GPIO_ERROR_DIRECTION;
    }
}
