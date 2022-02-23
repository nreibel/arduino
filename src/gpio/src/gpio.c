#include "gpio.h"
#include "bits.h"
#include "types.h"
#include "os_cfg.h"

#include <avr/interrupt.h>
#include <util/delay.h>

/* Private data */

static volatile void* extint_data[NUMBER_OF_EXTINTS] = {0};
static gpio_extint_cbk_t extint_cbk[NUMBER_OF_EXTINTS] = {0};

static volatile void* pcint_data[NUMBER_OF_PCINTS] = {0};
static gpio_pcint_cbk_t pcint_cbk[NUMBER_OF_PCINTS] = {0};

/* Interrupt routines */

ISR(INT0_vect)
{
    if (extint_cbk[EXTINT_0] != NULL_PTR)
    {
        (*extint_cbk[EXTINT_0])(EXTINT_0, extint_data[EXTINT_0]);
    }
}

ISR(INT1_vect)
{
    if (extint_cbk[EXTINT_1] != NULL_PTR)
    {
        (*extint_cbk[EXTINT_1])(EXTINT_1, extint_data[EXTINT_1]);
    }
}

ISR(PCINT0_vect)
{
    if (pcint_cbk[PORT_B] != NULL_PTR)
    {
        (*pcint_cbk[PORT_B])(PORT_B, PORTS[PORT_B].PIN, pcint_data[PORT_B]);
    }
}

// ISR(PCINT1_vect)
// {
//     if (pcint_cbk[PORT_C] != NULL_PTR)
//     {
//         (*pcint_cbk[PORT_C])(PORT_C, PORTS[PORT_C].PIN, pcint_data[PORT_C]);
//     }
// }
//
// ISR(PCINT2_vect)
// {
//     if (pcint_cbk[PORT_D] != NULL_PTR)
//     {
//         (*pcint_cbk[PORT_D])(PORT_D, PORTS[PORT_D].PIN, pcint_data[PORT_D]);
//     }
// }

/* External interrupts */

int gpio_enable_pcint(pcint_t port, uint8_t mask, gpio_pcint_cbk_t cbk, volatile void *data)
{
    switch(port)
    {
        case PCINT_B: // PCINT0..7:
        case PCINT_C: // PCINT8..14
        case PCINT_D: // PCINT16..23
            PCMSK[port] = mask;
            SET_BIT(PCICR, port);
            break;

        default:
            return -GPIO_INVALID_PORT;
    }

    pcint_cbk[port] = cbk;
    pcint_data[port] = data;

    return GPIO_OK;
}

int gpio_disable_pcint(pcint_t port)
{
    switch(port)
    {
        case PCINT_B: // PCINT0..7:
        case PCINT_C: // PCINT8..14
        case PCINT_D: // PCINT16..23
            RESET_BIT(PCICR, port);
            PCMSK[port] = 0;
            break;

        default:
            return -GPIO_INVALID_PORT;
    }

    pcint_cbk[port] = NULL_PTR;
    pcint_data[port] = NULL_PTR;

    return GPIO_OK;
}

int gpio_enable_extint(extint_t pin, gpio_edge_t edge, gpio_extint_cbk_t cbk, volatile void *data)
{
    switch(pin)
    {
        case EXTINT_0:
        case EXTINT_1:
        {
            switch(edge)
            {
                case GPIO_EDGE_LOW:
                case GPIO_EDGE_ANY:
                case GPIO_EDGE_FALLING:
                case GPIO_EDGE_RISING:
                    SET_MASK(EICRA, MASK(edge, 0x3) << (2*pin));
                    SET_BIT(EIMSK, pin);
                    break;

                default:
                    return -GPIO_INVALID_MODE;
            }

            break;
        }

        default:
            return -GPIO_INVALID_INT;
    }

    extint_data[pin] = data;
    extint_cbk[pin] = cbk;

    return GPIO_OK;
}

int gpio_disable_extint(extint_t pin)
{
    switch(pin)
    {
        case EXTINT_0:
        case EXTINT_1:
            RESET_BIT(EIMSK, pin);
            RESET_MASK(EICRA, 0x3 << (2*pin));
            break;

        default:
            return -GPIO_INVALID_INT;
    }

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

    // Set data direction
    switch(direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
        case GPIO_OUTPUT_ACTIVE_LOW:
            SET_BIT(PORTS[port].DDR, pin);
            break;

        case GPIO_INPUT_HIGH_Z:
        case GPIO_INPUT_PULLUP:
            RESET_BIT(PORTS[port].DDR, pin);
            break;

        default:
            return -GPIO_INVALID_MODE;
    }

    // Set pin value
    switch(direction)
    {
        case GPIO_OUTPUT_ACTIVE_LOW:
        case GPIO_INPUT_PULLUP:
            SET_BIT(PORTS[port].PORT, pin);
            break;

        case GPIO_OUTPUT_ACTIVE_HIGH:
        case GPIO_INPUT_HIGH_Z:
            RESET_BIT(PORTS[port].PORT, pin);
            break;

        default:
            return -GPIO_INVALID_MODE;
    }

    self->port = port;
    self->pin = pin;
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
            break;

        default:
            return -GPIO_INVALID_MODE;
    }

    return GPIO_OK;
}

int gpio_set(gpio_t self)
{
    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
            SET_BIT(PORTS[self->port].PORT, self->pin);
            break;

        case GPIO_OUTPUT_ACTIVE_LOW:
            RESET_BIT(PORTS[self->port].PORT, self->pin);
            break;

        default:
            return -GPIO_INVALID_MODE;
    }

    return GPIO_OK;
}

int gpio_reset(gpio_t self)
{
    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
            RESET_BIT(PORTS[self->port].PORT, self->pin);
            break;

        case GPIO_OUTPUT_ACTIVE_LOW:
            SET_BIT(PORTS[self->port].PORT, self->pin);
            break;

        default:
            return -GPIO_INVALID_MODE;
    }

    return GPIO_OK;
}
