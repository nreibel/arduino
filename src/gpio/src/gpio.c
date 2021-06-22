#include "gpio.h"
#include "bits.h"
#include "types.h"
#include "os_cfg.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile void* extint_data[NBR_OF_EXTINT] = {0};
static gpio_extint_cbk_t extint_cbk[NBR_OF_EXTINT] = {0};

static volatile void* pcint_data[NBR_OF_PORTS] = {0};
static gpio_pcint_cbk_t pcint_cbk[NBR_OF_PORTS] = {0};

ISR(INT0_vect)
{
    if (extint_cbk[0] != NULL_PTR)
    {
        (*extint_cbk[0])(extint_data[0]);
    }
}

ISR(INT1_vect)
{
    if (extint_cbk[1] != NULL_PTR)
    {
        (*extint_cbk[1])(extint_data[1]);
    }
}

ISR(PCINT0_vect)
{
    if (pcint_cbk[0] != NULL_PTR)
    {
        (*pcint_cbk[0])(PINB, pcint_data[0]);
    }
}

ISR(PCINT1_vect)
{
    if (pcint_cbk[1] != NULL_PTR)
    {
        (*pcint_cbk[1])(PINC, pcint_data[1]);
    }
}

ISR(PCINT2_vect)
{
    if (pcint_cbk[2] != NULL_PTR)
    {
        (*pcint_cbk[2])(PIND, pcint_data[2]);
    }
}

int gpio_enable_pcint(gpio_port_t port, uint8_t mask, gpio_pcint_cbk_t cbk, volatile void *data)
{
    switch(port)
    {
        case GPIO_PORT_B: // PCINT0..7:
        {
            pcint_cbk[0] = cbk;
            pcint_data[0] = data;

            PCMSK0 = mask;
            SET_BIT(PCICR, PCIE0);
            break;
        }

        case GPIO_PORT_C: // PCINT8..14
        {
            pcint_cbk[1] = cbk;
            pcint_data[1] = data;

            PCMSK1 = mask;
            SET_BIT(PCICR, PCIE1);
            break;
        }

        case GPIO_PORT_D: // PCINT16..23
        {
            pcint_cbk[2] = cbk;
            pcint_data[2] = data;

            PCMSK2 = mask;
            SET_BIT(PCICR, PCIE2);
            break;
        }

        default: return -1;
    }

    return 0;
}

int gpio_enable_extint(gpio_int_t pin, gpio_edge_t edge, gpio_extint_cbk_t cbk, volatile void *data)
{
    uint8_t eicra = EICRA;
    uint8_t eimsk = EIMSK;

    switch(pin)
    {
        case GPIO_INT_0:
        {
            switch(edge)
            {
                case GPIO_EDGE_LOW:
                    RESET_BIT(eicra, ISC01);
                    RESET_BIT(eicra, ISC00);
                    break;

                case GPIO_EDGE_ANY:
                    RESET_BIT(eicra, ISC01);
                    SET_BIT(eicra, ISC00);
                    break;

                case GPIO_EDGE_FALLING:
                    SET_BIT(eicra, ISC01);
                    RESET_BIT(eicra, ISC00);
                    break;

                case GPIO_EDGE_RISING:
                    SET_BIT(eicra, ISC01);
                    SET_BIT(eicra, ISC00);
                    break;

                default:
                    return -2;
            }

            SET_BIT(eimsk, INT0);

            extint_data[0] = data;
            extint_cbk[0] = cbk;

            break;
        }

        case GPIO_INT_1:
        {
            switch(edge)
            {
                case GPIO_EDGE_LOW:
                    RESET_BIT(eicra, ISC11);
                    RESET_BIT(eicra, ISC10);
                    break;

                case GPIO_EDGE_ANY:
                    RESET_BIT(eicra, ISC11);
                    SET_BIT(eicra, ISC10);
                    break;

                case GPIO_EDGE_FALLING:
                    SET_BIT(eicra, ISC11);
                    RESET_BIT(eicra, ISC10);
                    break;

                case GPIO_EDGE_RISING:
                    SET_BIT(eicra, ISC11);
                    SET_BIT(eicra, ISC10);
                    break;

                default:
                    return -2;
            }

            SET_BIT(eimsk, INT1);

            extint_data[1] = data;
            extint_cbk[1] = cbk;

            break;
        }

        default:
        {
            return -1;
        }
    }

    EICRA = eicra;
    EIMSK = eimsk;

    return 0;
}

int gpio_init(gpio_t *self, gpio_port_t port, uint8_t pin, gpio_data_direction_t direction)
{
    self->port = port;
    self->pin = pin;
    self->direction = direction;

    if (self->pin >= 8) return -1;

    // Set DDR
    switch(direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
        case GPIO_OUTPUT_ACTIVE_LOW:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: SET_BIT(DDRB, self->pin); break;
                case GPIO_PORT_C: SET_BIT(DDRC, self->pin); break;
                case GPIO_PORT_D: SET_BIT(DDRD, self->pin); break;
                default: return -1;
            }
            break;
        }

        case GPIO_INPUT_HIGH_Z:
        case GPIO_INPUT_PULLUP:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: RESET_BIT(DDRB, self->pin); break;
                case GPIO_PORT_C: RESET_BIT(DDRC, self->pin); break;
                case GPIO_PORT_D: RESET_BIT(DDRD, self->pin); break;
                default: return -1;
            }
            break;
        }

        default: return -1;
    }

    // Set PORT
    switch(direction)
    {
        case GPIO_OUTPUT_ACTIVE_LOW:
        case GPIO_INPUT_PULLUP:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: SET_BIT(PORTB, self->pin); break;
                case GPIO_PORT_C: SET_BIT(PORTC, self->pin); break;
                case GPIO_PORT_D: SET_BIT(PORTD, self->pin); break;
                default: return -1;
            }
            break;
        }

        case GPIO_OUTPUT_ACTIVE_HIGH:
        case GPIO_INPUT_HIGH_Z:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: RESET_BIT(PORTB, self->pin); break;
                case GPIO_PORT_C: RESET_BIT(PORTC, self->pin); break;
                case GPIO_PORT_D: RESET_BIT(PORTD, self->pin); break;
                default: return -1;
            }
            break;
        }

        default: return -1;
    }

    return 0;
}

int gpio_get(gpio_t *self, bool *state)
{
    if (self->pin >= 8) return -1;

    switch(self->direction)
    {
        case GPIO_INPUT_HIGH_Z:
        case GPIO_INPUT_PULLUP:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: *state = IS_SET_BIT(PINB, self->pin) ? TRUE : FALSE; break;
                case GPIO_PORT_C: *state = IS_SET_BIT(PINC, self->pin) ? TRUE : FALSE; break;
                case GPIO_PORT_D: *state = IS_SET_BIT(PIND, self->pin) ? TRUE : FALSE; break;
                default: return -1;
            }
            break;
        }

        default: return -2;
    }

    return 0;
}

int gpio_set(gpio_t *self)
{
    if (self->pin >= 8) return -1;

    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: SET_BIT(PORTB, self->pin); break;
                case GPIO_PORT_C: SET_BIT(PORTC, self->pin); break;
                case GPIO_PORT_D: SET_BIT(PORTD, self->pin); break;
                default: return -1;
            }
            break;
        }

        case GPIO_OUTPUT_ACTIVE_LOW:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: RESET_BIT(PORTB, self->pin); break;
                case GPIO_PORT_C: RESET_BIT(PORTC, self->pin); break;
                case GPIO_PORT_D: RESET_BIT(PORTD, self->pin); break;
                default: return -1;
            }
            break;
        }

        default: return -2;
    }

    return 0;
}

int gpio_reset(gpio_t *self)
{
    if (self->pin >= 8) return -1;

    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: RESET_BIT(PORTB, self->pin); break;
                case GPIO_PORT_C: RESET_BIT(PORTC, self->pin); break;
                case GPIO_PORT_D: RESET_BIT(PORTD, self->pin); break;
                default: return -1;
            }
            break;
        }

        case GPIO_OUTPUT_ACTIVE_LOW:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: SET_BIT(PORTB, self->pin); break;
                case GPIO_PORT_C: SET_BIT(PORTC, self->pin); break;
                case GPIO_PORT_D: SET_BIT(PORTD, self->pin); break;
                default: return -1;
            }
            break;
        }

        default: return -2;
    }

    return 0;
}
