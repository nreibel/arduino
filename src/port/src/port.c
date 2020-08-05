#include "port.h"
#include "bits.h"
#include "types.h"
#include "os_cfg.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define PORT_NUMBER_OF_INT 2

static volatile void* Data_INT[PORT_NUMBER_OF_INT] = {NULL_PTR};
static Interrupt Interrupts_INT[PORT_NUMBER_OF_INT] = {NULL_PTR};

ISR(INT0_vect)
{
    if (Interrupts_INT[0] != NULL_PTR)
    {
        ( *Interrupts_INT[0] )( Data_INT[0] );
    }
}

ISR(INT1_vect)
{
    if (Interrupts_INT[1] != NULL_PTR)
    {
        ( *Interrupts_INT[1] )( Data_INT[1] );
    }
}

Std_ReturnType Port_EnableInt(ExtInt input, Edge edge, Interrupt cbk, volatile void *data)
{
    switch(input)
    {
        case ExtInt_0:
            Data_INT[0] = data;
            Interrupts_INT[0] = cbk;
            SET_BITS(EICRA, edge, 0x3);
            SET_BIT(EIMSK, INT0);
            break;
        case ExtInt_1:
            Data_INT[1] = data;
            Interrupts_INT[1] = cbk;
            SET_BITS(EICRA, edge << 2, 0xC);
            SET_BIT(EIMSK, INT1);
            break;
        default:
            HALT; // Should not get here
    }

    return Status_OK;
}

Std_ReturnType Port_RisingEdge(GPIO pin)
{
    Port_SetPinState(pin, High);
    _delay_us(1);
    Port_SetPinState(pin, Low);
    return Status_OK;
}

Std_ReturnType Port_FallingEdge(GPIO pin)
{
    Port_SetPinState(pin, Low);
    _delay_us(1);
    Port_SetPinState(pin, High);
    return Status_OK;
}

Std_ReturnType Port_SetDataDirection(Port port, uint8_t direction)
{
    Std_ReturnType status = Status_Not_OK;

    switch(port)
    {
        case Port_B:
            DDRB = direction;
            status = Status_OK;
            break;

        case Port_C:
            DDRC = direction;
            status = Status_OK;
            break;

        case Port_D:
            DDRD = direction;
            status = Status_OK;
            break;
    }

    return status;
}

Std_ReturnType Port_GetDataDirection(Port port, uint8_t* direction)
{
    Std_ReturnType status = Status_Not_OK;

    switch(port)
    {
        case Port_B:
            *direction = DDRB;
            status = Status_OK;
            break;

        case Port_C:
            *direction = DDRC;
            status = Status_OK;
            break;

        case Port_D:
            *direction = DDRD;
            status = Status_OK;
            break;
    }

    return status;
}

Std_ReturnType Port_SetValue(Port port, uint8_t value)
{
    Std_ReturnType status = Status_Not_OK;

    switch(port)
    {
        case Port_B:
            PORTB = value;
            status = Status_OK;
            break;

        case Port_C:
            PORTC = value;
            status = Status_OK;
            break;

        case Port_D:
            PORTD = value;
            status = Status_OK;
            break;
    }

    return status;
}

Std_ReturnType Port_GetValue(Port port, uint8_t* value)
{
    Std_ReturnType status = Status_Not_OK;

    switch(port)
    {
        case Port_B:
            *value = PINB;
            status = Status_OK;
            break;

        case Port_C:
            *value = PINC;
            status = Status_OK;
            break;

        case Port_D:
            *value = PIND;
            status = Status_OK;
            break;
    }

    return status;
}

Std_ReturnType Port_SetPinDataDirection(GPIO pinDef, DataDirection direction)
{
    Std_ReturnType status = Status_Not_OK;

    uint8_t reg;
    status = Port_GetDataDirection(pinDef.port, &reg);
    if (status == Status_OK)
    {
        switch (direction)
        {
            case Input:
                RESET_BIT(reg, pinDef.pin);
                status = Port_SetDataDirection(pinDef.port, reg);
                break;
            case Output:
                SET_BIT(reg, pinDef.pin);
                status = Port_SetDataDirection(pinDef.port, reg);
                break;
        }
    }

    return status;
}

Std_ReturnType Port_GetPinDataDirection(GPIO pinDef, DataDirection *direction)
{
    Std_ReturnType status = Status_Not_OK;

    uint8_t reg;
    status = Port_GetDataDirection(pinDef.port, &reg);
    if (status == Status_OK)
    {
        *direction = IS_SET_BIT(reg, pinDef.pin) ? Output : Input;
    }

    return status;
}

Std_ReturnType Port_SetPinState(GPIO pinDef, State pinState)
{
    Std_ReturnType status = Status_Not_OK;

    uint8_t reg;
    status = Port_GetValue(pinDef.port, &reg);
    if (status == Status_OK)
    {
        switch(pinState)
        {
            case Low:
                RESET_BIT(reg, pinDef.pin);
                status = Port_SetValue(pinDef.port, reg);
                break;
            case High:
                SET_BIT(reg, pinDef.pin);
                status = Port_SetValue(pinDef.port, reg);
                break;
        }
    }

    return status;
}

Std_ReturnType Port_GetPinState(GPIO pinDef, State *pinState)
{
    Std_ReturnType status = Status_Not_OK;

    uint8_t reg;
    status = Port_GetValue(pinDef.port, &reg);
    if (status == Status_OK)
    {
        *pinState = IS_SET_BIT(reg, pinDef.pin) ? High : Low;
    }

    return status;
}

