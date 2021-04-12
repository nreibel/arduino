#include "gpio.h"
#include "bits.h"
#include "types.h"
#include "os_cfg.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define NBR_OF_INT_INPUTS 2

static volatile void* int_data[NBR_OF_INT_INPUTS] = {0};
static gpio_interrupt_callback_t int_cbk[NBR_OF_INT_INPUTS] = {0};

ISR(INT0_vect)
{
    if (int_cbk[0] != NULL_PTR)
    {
        (*int_cbk[0])(int_data[0]);
    }
}

ISR(INT1_vect)
{
    if (int_cbk[1] != NULL_PTR)
    {
        (*int_cbk[1])(int_data[1]);
    }
}

int gpio_enable_interrupt(gpio_t *self, gpio_edge_t edge, gpio_interrupt_callback_t cbk, volatile void *data)
{
    uint8_t eicra = EICRA;
    uint8_t eimsk = EIMSK;

    switch(self->port)
    {
        case GPIO_PORT_B: return -1;
        case GPIO_PORT_C: return -1;
        case GPIO_PORT_D:
        {
            switch(self->pin)
            {
                case 0: return -1;
                case 1: return -1;
                case 2: // INT0
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

                    int_data[0] = data;
                    int_cbk[0] = cbk;

                    break;
                }
                case 3: // INT1
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

                    int_data[1] = data;
                    int_cbk[1] = cbk;

                    break;
                }
                case 4: return -1;
                case 5: return -1;
                case 6: return -1;
                case 7: return -1;
                default: return -2;
            }
            break;
        }
        default: return -2;
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
