#include "gpio.h"
#include "bits.h"
#include "types.h"
#include "os_cfg.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile void* Data_INT0 = NULL_PTR;
static volatile void* Data_INT1 = NULL_PTR;

static Interrupt Interrupts_INT0 = NULL_PTR;
static Interrupt Interrupts_INT1 = NULL_PTR;

ISR(INT0_vect)
{
    if (Interrupts_INT0 != NULL_PTR)
    {
        (*Interrupts_INT0)(Data_INT0);
    }
}

ISR(INT1_vect)
{
    if (Interrupts_INT1 != NULL_PTR)
    {
        (*Interrupts_INT1)(Data_INT1);
    }
}

Std_ReturnType GPIO_EnableInterrupt(GPIO pin, GPIO_Edge edge, Interrupt cbk, volatile void *data)
{
    uint8_t eicra = EICRA;
    uint8_t eimsk = EIMSK;

    switch(pin)
    {
        case GPIO_INT0:
        {
            SET_BIT(eimsk, INT0);

            switch(edge)
            {
                case GPIO_Edge_Low:
                    RESET_BIT(eicra, ISC01);
                    RESET_BIT(eicra, ISC00);
                    break;
                case GPIO_Edge_Both:
                    RESET_BIT(eicra, ISC01);
                    SET_BIT(eicra, ISC00);
                    break;
                case GPIO_Edge_Falling:
                    SET_BIT(eicra, ISC01);
                    RESET_BIT(eicra, ISC00);
                    break;
                case GPIO_Edge_Rising:
                    SET_BIT(eicra, ISC01);
                    SET_BIT(eicra, ISC00);
                    break;
                default:
                    return Status_Not_OK;
            }

            Data_INT0 = data;
            Interrupts_INT0 = cbk;

            break;
        }
        case GPIO_INT1:
        {
            SET_BIT(eimsk, INT1);

            switch(edge)
            {
                case GPIO_Edge_Low:
                    RESET_BIT(eicra, ISC11);
                    RESET_BIT(eicra, ISC10);
                    break;
                case GPIO_Edge_Both:
                    RESET_BIT(eicra, ISC11);
                    SET_BIT(eicra, ISC10);
                    break;
                case GPIO_Edge_Falling:
                    SET_BIT(eicra, ISC11);
                    RESET_BIT(eicra, ISC10);
                    break;
                case GPIO_Edge_Rising:
                    SET_BIT(eicra, ISC11);
                    SET_BIT(eicra, ISC10);
                    break;
                default:
                    return Status_Not_OK;
            }

            Data_INT1 = data;
            Interrupts_INT1 = cbk;

            break;
        }
        default: return Status_Not_OK;
    }

    EICRA = eicra;
    EIMSK = eimsk;

    return Status_OK;
}

Std_ReturnType GPIO_SetDataDirection(GPIO pin, GPIO_DataDirection direction)
{
    switch(pin)
    {
        case GPIO_D0:
        case GPIO_D1:
        case GPIO_D2:
        case GPIO_D3:
        case GPIO_D4:
        case GPIO_D5:
        case GPIO_D6:
        case GPIO_D7:
            if (direction == GPIO_Output) SET_BIT(DDRD, pin - GPIO_D0);
            else RESET_BIT(DDRD, pin - GPIO_D0);
            break;

        case GPIO_D8:
        case GPIO_D9:
        case GPIO_D10:
        case GPIO_D11:
        case GPIO_D12:
        case GPIO_D13:
            if (direction == GPIO_Output) SET_BIT(DDRB, pin - GPIO_D8);
            else RESET_BIT(DDRB, pin - GPIO_D8);
            break;

        case GPIO_A0:
        case GPIO_A1:
        case GPIO_A2:
        case GPIO_A3:
        case GPIO_A4:
        case GPIO_A5:
            if (direction == GPIO_Output) SET_BIT(DDRC, pin - GPIO_A0);
            else RESET_BIT(DDRC, pin - GPIO_A0);
            break;

        default:
            return Status_Not_OK;
    }

    return Status_OK;
}

Std_ReturnType GPIO_GetState(GPIO pin, GPIO_State *state)
{
    switch(pin)
    {
        case GPIO_D0:
        case GPIO_D1:
        case GPIO_D2:
        case GPIO_D3:
        case GPIO_D4:
        case GPIO_D5:
        case GPIO_D6:
        case GPIO_D7:
            *state = IS_SET_BIT(PIND, pin - GPIO_D0) ? GPIO_High : GPIO_Low;
            break;

        case GPIO_D8:
        case GPIO_D9:
        case GPIO_D10:
        case GPIO_D11:
        case GPIO_D12:
        case GPIO_D13:
            *state = IS_SET_BIT(PINB, pin - GPIO_D8) ? GPIO_High : GPIO_Low;
            break;

        case GPIO_A0:
        case GPIO_A1:
        case GPIO_A2:
        case GPIO_A3:
        case GPIO_A4:
        case GPIO_A5:
            *state = IS_SET_BIT(PINC, pin - GPIO_A0) ? GPIO_High : GPIO_Low;
            break;

        default:
            return Status_Not_OK;
    }

    return Status_OK;
}

Std_ReturnType GPIO_SetState(GPIO pin, GPIO_State state)
{
    switch(pin)
    {
        case GPIO_D0:
        case GPIO_D1:
        case GPIO_D2:
        case GPIO_D3:
        case GPIO_D4:
        case GPIO_D5:
        case GPIO_D6:
        case GPIO_D7:
            if (state == GPIO_High) SET_BIT(PORTD, pin - GPIO_D0);
            else RESET_BIT(PORTD, pin - GPIO_D0);
            break;

        case GPIO_D8:
        case GPIO_D9:
        case GPIO_D10:
        case GPIO_D11:
        case GPIO_D12:
        case GPIO_D13:
            if (state == GPIO_High) SET_BIT(PORTB, pin - GPIO_D8);
            else RESET_BIT(PORTB, pin - GPIO_D8);
            break;

        case GPIO_A0:
        case GPIO_A1:
        case GPIO_A2:
        case GPIO_A3:
        case GPIO_A4:
        case GPIO_A5:
            if (state == GPIO_High) SET_BIT(PORTC, pin - GPIO_A0);
            else RESET_BIT(PORTC, pin - GPIO_A0);
            break;

        default:
            return Status_Not_OK;
    }

    return Status_OK;
}

Std_ReturnType GPIO_RisingEdge(GPIO pin)
{
    GPIO_SetState(pin, TRUE);
    _delay_us(1);
    GPIO_SetState(pin, FALSE);
    return Status_OK;
}

Std_ReturnType GPIO_FallingEdge(GPIO pin)
{
    GPIO_SetState(pin, FALSE);
    _delay_us(1);
    GPIO_SetState(pin, TRUE);
    return Status_OK;
}
