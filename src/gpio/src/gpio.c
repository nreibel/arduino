#include "gpio.h"
#include "bits.h"
#include "types.h"
#include "os_cfg.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


/*
 * Private data
 */

static volatile void* extint_data[NBR_OF_EXTINT] = {0};
static gpio_extint_cbk_t extint_cbk[NBR_OF_EXTINT] = {0};

static volatile void* pcint_data[NBR_OF_PORTS] = {0};
static gpio_pcint_cbk_t pcint_cbk[NBR_OF_PORTS] = {0};

static struct gpio_prv_s _PB0 = { .port = GPIO_PORT_B, .pin = 0 };
static struct gpio_prv_s _PB1 = { .port = GPIO_PORT_B, .pin = 1 };
static struct gpio_prv_s _PB2 = { .port = GPIO_PORT_B, .pin = 2 };
static struct gpio_prv_s _PB3 = { .port = GPIO_PORT_B, .pin = 3 };
static struct gpio_prv_s _PB4 = { .port = GPIO_PORT_B, .pin = 4 };
static struct gpio_prv_s _PB5 = { .port = GPIO_PORT_B, .pin = 5 };
static struct gpio_prv_s _PC0 = { .port = GPIO_PORT_C, .pin = 0 };
static struct gpio_prv_s _PC1 = { .port = GPIO_PORT_C, .pin = 1 };
static struct gpio_prv_s _PC2 = { .port = GPIO_PORT_C, .pin = 2 };
static struct gpio_prv_s _PC3 = { .port = GPIO_PORT_C, .pin = 3 };
static struct gpio_prv_s _PC4 = { .port = GPIO_PORT_C, .pin = 4 };
static struct gpio_prv_s _PC5 = { .port = GPIO_PORT_C, .pin = 5 };
static struct gpio_prv_s _PD0 = { .port = GPIO_PORT_D, .pin = 0 };
static struct gpio_prv_s _PD1 = { .port = GPIO_PORT_D, .pin = 1 };
static struct gpio_prv_s _PD2 = { .port = GPIO_PORT_D, .pin = 2 };
static struct gpio_prv_s _PD3 = { .port = GPIO_PORT_D, .pin = 3 };
static struct gpio_prv_s _PD4 = { .port = GPIO_PORT_D, .pin = 4 };
static struct gpio_prv_s _PD5 = { .port = GPIO_PORT_D, .pin = 5 };
static struct gpio_prv_s _PD6 = { .port = GPIO_PORT_D, .pin = 6 };
static struct gpio_prv_s _PD7 = { .port = GPIO_PORT_D, .pin = 7 };

/*
 * Public constants
 */

// Analog inputs
const gpio_t A0 = &_PC0;
const gpio_t A1 = &_PC1;
const gpio_t A2 = &_PC2;
const gpio_t A3 = &_PC3;
const gpio_t A4 = &_PC4;
const gpio_t A5 = &_PC5;

// Digital inputs
const gpio_t D0  = &_PD0;
const gpio_t D1  = &_PD1;
const gpio_t D2  = &_PD2;
const gpio_t D3  = &_PD3;
const gpio_t D4  = &_PD4;
const gpio_t D5  = &_PD5;
const gpio_t D6  = &_PD6;
const gpio_t D7  = &_PD7;
const gpio_t D8  = &_PB0;
const gpio_t D9  = &_PB1;
const gpio_t D10 = &_PB2;
const gpio_t D11 = &_PB3;
const gpio_t D12 = &_PB4;
const gpio_t D13 = &_PB5;

// Aliases
const gpio_t RX   = &_PD0;
const gpio_t TX   = &_PD1;
const gpio_t CS   = &_PB2;
const gpio_t MOSI = &_PB3;
const gpio_t MISO = &_PB4;
const gpio_t SCK  = &_PB5;
const gpio_t LED  = &_PB5;
const gpio_t SDA  = &_PC4;
const gpio_t SCL  = &_PC5;

/*
 * Interrupt routines
 */

ISR(INT0_vect)
{
    if (extint_cbk[0] != NULL_PTR)
    {
        (*extint_cbk[0])(GPIO_INT_0, extint_data[0]);
    }
}

ISR(INT1_vect)
{
    if (extint_cbk[1] != NULL_PTR)
    {
        (*extint_cbk[1])(GPIO_INT_1, extint_data[1]);
    }
}

ISR(PCINT0_vect)
{
    if (pcint_cbk[0] != NULL_PTR)
    {
        (*pcint_cbk[0])(GPIO_PORT_B, PINB, pcint_data[0]);
    }
}

ISR(PCINT1_vect)
{
    if (pcint_cbk[1] != NULL_PTR)
    {
        (*pcint_cbk[1])(GPIO_PORT_C, PINC, pcint_data[1]);
    }
}

ISR(PCINT2_vect)
{
    if (pcint_cbk[2] != NULL_PTR)
    {
        (*pcint_cbk[2])(GPIO_PORT_D, PIND, pcint_data[2]);
    }
}

/*
 * External interrupts
 */

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

        default: return -GPIO_INVALID_PORT;
    }

    return GPIO_OK;
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
                    return -GPIO_INVALID_MODE;
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
                    return -GPIO_INVALID_MODE;
            }

            SET_BIT(eimsk, INT1);

            extint_data[1] = data;
            extint_cbk[1] = cbk;

            break;
        }

        default:
        {
            return -GPIO_INVALID_INT;
        }
    }

    EICRA = eicra;
    EIMSK = eimsk;

    return GPIO_OK;
}

/*
 * Public functions
 */

int gpio_init(gpio_t self, gpio_port_t port, uint8_t pin)
{
    self->port = port;
    self->pin = pin;
    self->direction = GPIO_DIRECTION_NONE;

    return GPIO_OK;
}

int gpio_set_data_direction(gpio_t self, gpio_data_direction_t direction)
{
    self->direction = direction;

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
                default: return -GPIO_INVALID_PORT;
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
                default: return -GPIO_INVALID_PORT;
            }
            break;
        }

        default: return -GPIO_INVALID_MODE;
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
                default: return -GPIO_INVALID_PORT;
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
                default: return -GPIO_INVALID_PORT;
            }
            break;
        }

        default: return -GPIO_INVALID_MODE;
    }

    return GPIO_OK;
}

int gpio_get(gpio_t self, bool *state)
{
    if (self->pin >= 8) return -GPIO_INVALID_PIN;

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
                default: return -GPIO_INVALID_PORT;
            }
            break;
        }

        default: return -GPIO_INVALID_MODE;
    }

    return GPIO_OK;
}

int gpio_set(gpio_t self)
{
    if (self->pin >= 8) return -GPIO_INVALID_PIN;

    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: SET_BIT(PORTB, self->pin); break;
                case GPIO_PORT_C: SET_BIT(PORTC, self->pin); break;
                case GPIO_PORT_D: SET_BIT(PORTD, self->pin); break;
                default: return -GPIO_INVALID_PORT;
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
                default: return -GPIO_INVALID_PORT;
            }
            break;
        }

        default: return -GPIO_INVALID_MODE;
    }

    return GPIO_OK;
}

int gpio_reset(gpio_t self)
{
    if (self->pin >= 8) return -GPIO_INVALID_PIN;

    switch(self->direction)
    {
        case GPIO_OUTPUT_ACTIVE_HIGH:
        {
            switch(self->port)
            {
                case GPIO_PORT_B: RESET_BIT(PORTB, self->pin); break;
                case GPIO_PORT_C: RESET_BIT(PORTC, self->pin); break;
                case GPIO_PORT_D: RESET_BIT(PORTD, self->pin); break;
                default: return -GPIO_INVALID_PORT;
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
                default: return -GPIO_INVALID_PORT;
            }
            break;
        }

        default: return -GPIO_INVALID_MODE;
    }

    return GPIO_OK;
}
