#include "gpio.h"
#include "bits.h"
#include "types.h"
#include "os_cfg.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile void* int0_data = NULL_PTR;
static volatile void* int1_data = NULL_PTR;

static Interrupt int0_cbk = NULL_PTR;
static Interrupt int1_cbk = NULL_PTR;

ISR(INT0_vect)
{
    if (int0_cbk != NULL_PTR)
    {
        (*int0_cbk)(int0_data);
    }
}

ISR(INT1_vect)
{
    if (int1_cbk != NULL_PTR)
    {
        (*int1_cbk)(int1_data);
    }
}

// Std_ReturnType GPIO_EnableInterrupt(GPIO pin, GPIO_Edge edge, Interrupt cbk, volatile void *data)
// {
//     uint8_t eicra = EICRA;
//     uint8_t eimsk = EIMSK;
//
//     switch(pin)
//     {
//         case GPIO_INT0:
//         {
//             SET_BIT(eimsk, INT0);
//
//             switch(edge)
//             {
//                 case GPIO_Edge_Low:
//                     RESET_BIT(eicra, ISC01);
//                     RESET_BIT(eicra, ISC00);
//                     break;
//                 case GPIO_Edge_Any:
//                     RESET_BIT(eicra, ISC01);
//                     SET_BIT(eicra, ISC00);
//                     break;
//                 case GPIO_Edge_Falling:
//                     SET_BIT(eicra, ISC01);
//                     RESET_BIT(eicra, ISC00);
//                     break;
//                 case GPIO_Edge_Rising:
//                     SET_BIT(eicra, ISC01);
//                     SET_BIT(eicra, ISC00);
//                     break;
//                 default:
//                     return Status_Not_OK;
//             }
//
//             Data_INT0 = data;
//             Interrupts_INT0 = cbk;
//
//             break;
//         }
//         case GPIO_INT1:
//         {
//             SET_BIT(eimsk, INT1);
//
//             switch(edge)
//             {
//                 case GPIO_Edge_Low:
//                     RESET_BIT(eicra, ISC11);
//                     RESET_BIT(eicra, ISC10);
//                     break;
//                 case GPIO_Edge_Any:
//                     RESET_BIT(eicra, ISC11);
//                     SET_BIT(eicra, ISC10);
//                     break;
//                 case GPIO_Edge_Falling:
//                     SET_BIT(eicra, ISC11);
//                     RESET_BIT(eicra, ISC10);
//                     break;
//                 case GPIO_Edge_Rising:
//                     SET_BIT(eicra, ISC11);
//                     SET_BIT(eicra, ISC10);
//                     break;
//                 default:
//                     return Status_Not_OK;
//             }
//
//             Data_INT1 = data;
//             Interrupts_INT1 = cbk;
//
//             break;
//         }
//         default: return Status_Not_OK;
//     }
//
//     EICRA = eicra;
//     EIMSK = eimsk;
//
//     return Status_OK;
// }

void gpio_init(gpio_t *self, gpio_port_t port, uint8_t pin)
{
    self->port = port;
    self->pin = pin;
}

void gpio_set_data_direction(gpio_t *self, gpio_data_direction_t direction)
{
    switch(self->port)
    {
        case GPIO_PORT_B:
        {
            switch(direction)
            {
                case GPIO_OUTPUT:
                    SET_BIT(DDRB, self->pin);
                    break;
                case GPIO_INPUT:
                    RESET_BIT(DDRB, self->pin);
                    RESET_BIT(PORTB, self->pin);
                    break;
                case GPIO_INPUT_PULLUP:
                    RESET_BIT(DDRB, self->pin);
                    SET_BIT(PORTB, self->pin);
                    break;
            }
            break;
        }
        case GPIO_PORT_C:
        {
            switch(direction)
            {
                case GPIO_OUTPUT:
                    SET_BIT(DDRC, self->pin);
                    break;
                case GPIO_INPUT:
                    RESET_BIT(DDRC, self->pin);
                    RESET_BIT(PORTC, self->pin);
                    break;
                case GPIO_INPUT_PULLUP:
                    RESET_BIT(DDRC, self->pin);
                    SET_BIT(PORTC, self->pin);
                    break;
            }
            break;
        }
        case GPIO_PORT_D:
        {
            switch(direction)
            {
                case GPIO_OUTPUT:
                    SET_BIT(DDRD, self->pin);
                    break;
                case GPIO_INPUT:
                    RESET_BIT(DDRD, self->pin);
                    RESET_BIT(PORTD, self->pin);
                    break;
                case GPIO_INPUT_PULLUP:
                    RESET_BIT(DDRD, self->pin);
                    SET_BIT(PORTD, self->pin);
                    break;
            }
            break;
        }
    }
}

void gpio_get_state(gpio_t *self, bool *state)
{
    switch(self->port)
    {
        case GPIO_PORT_B:
            *state = IS_SET_BIT(PINB, self->pin);
            break;
        case GPIO_PORT_C:
            *state = IS_SET_BIT(PINC, self->pin);
            break;
        case GPIO_PORT_D:
            *state = IS_SET_BIT(PIND, self->pin);
            break;
    }
}

void gpio_set_state(gpio_t *self, bool state)
{
    switch(self->port)
    {
        case GPIO_PORT_B:
            state ? SET_BIT(PORTB, self->pin) : RESET_BIT(PORTB, self->pin);
            break;
        case GPIO_PORT_C:
            state ? SET_BIT(PORTC, self->pin) : RESET_BIT(PORTC, self->pin);
            break;
        case GPIO_PORT_D:
            state ? SET_BIT(PORTD, self->pin) : RESET_BIT(PORTD, self->pin);
            break;
    }
}

// Std_ReturnType GPIO_RisingEdge(GPIO pin)
// {
//     GPIO_SetState(pin, TRUE);
//     _delay_us(1);
//     GPIO_SetState(pin, FALSE);
//     return Status_OK;
// }
//
// Std_ReturnType GPIO_FallingEdge(GPIO pin)
// {
//     GPIO_SetState(pin, FALSE);
//     _delay_us(1);
//     GPIO_SetState(pin, TRUE);
//     return Status_OK;
// }
