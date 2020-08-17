#include "gpio.h"
#include "bits.h"
#include "types.h"
#include "os_cfg.h"

#include <avr/io.h>
#include <util/delay.h>

Std_ReturnType GPIO_SetDataDirection(GPIO pin, GPIO_DataDirection direction)
{
    switch(pin)
    {
        case D0:
        case D1:
        case D2:
        case D3:
        case D4:
        case D5:
        case D6:
        case D7:
            if (direction) SET_BIT(DDRD, pin - D0);
            else RESET_BIT(DDRD, pin - D0);
            break;

        case D8:
        case D9:
        case D10:
        case D11:
        case D12:
        case D13:
            if (direction) SET_BIT(DDRB, pin - D8);
            else RESET_BIT(DDRB, pin - D8);
            break;

        case A0:
        case A1:
        case A2:
        case A3:
        case A4:
        case A5:
            if (direction) SET_BIT(DDRC, pin - A0);
            else RESET_BIT(DDRC, pin - A0);
            break;

        default:
            return Status_Not_OK;
    }

    return Status_OK;
}

Std_ReturnType GPIO_Get(GPIO pin, bool *state)
{
    switch(pin)
    {
        case D0:
        case D1:
        case D2:
        case D3:
        case D4:
        case D5:
        case D6:
        case D7:
            *state = GET_BIT(PIND, pin - D0);
            break;

        case D8:
        case D9:
        case D10:
        case D11:
        case D12:
        case D13:
            *state = GET_BIT(PINB, pin - D8);
            break;

        case A0:
        case A1:
        case A2:
        case A3:
        case A4:
        case A5:
            *state = GET_BIT(PINC, pin - A0);
            break;

        default:
            return Status_Not_OK;
    }

    return Status_OK;
}

Std_ReturnType GPIO_Set(GPIO pin, bool state)
{
    switch(pin)
    {
        case D0:
        case D1:
        case D2:
        case D3:
        case D4:
        case D5:
        case D6:
        case D7:
            if (state) SET_BIT(PORTD, pin - D0);
            else RESET_BIT(PORTD, pin - D0);
            break;

        case D8:
        case D9:
        case D10:
        case D11:
        case D12:
        case D13:
            if (state) SET_BIT(PORTB, pin - D8);
            else RESET_BIT(PORTB, pin - D8);
            break;

        case A0:
        case A1:
        case A2:
        case A3:
        case A4:
        case A5:
            if (state) SET_BIT(PORTC, pin - A0);
            else RESET_BIT(PORTC, pin - A0);
            break;

        default:
            return Status_Not_OK;
    }

    return Status_OK;
}

Std_ReturnType GPIO_RisingEdge(GPIO pin)
{
    GPIO_Set(pin, TRUE);
    _delay_us(1);
    GPIO_Set(pin, FALSE);
    return Status_OK;
}

Std_ReturnType GPIO_FallingEdge(GPIO pin)
{
    GPIO_Set(pin, FALSE);
    _delay_us(1);
    GPIO_Set(pin, TRUE);
    return Status_OK;
}
