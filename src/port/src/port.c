#include "port.h"
#include "port_prv.h"
#include "bits.h"
#include "types.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static Callback Callback_INT0 = NULL_PTR;
static Callback Callback_INT1 = NULL_PTR;

ISR(INT0_vect)
{
    if (Callback_INT0 != NULL_PTR)
    {
        (*Callback_INT0)();
    }
}

ISR(INT1_vect)
{
    if (Callback_INT1 != NULL_PTR)
    {
        (*Callback_INT1)();
    }
}


Std_ReturnType Port_EnableInt0(Edge edge, Callback cbk)
{
    Callback_INT0 = cbk;

    switch(edge)
    {
        case Edge_LowLevel:
            SET_MASK(EICRA, 0);
            break;
        case Edge_Both:
            SET_MASK(EICRA, BIT(ISC00));
            break;
        case Edge_Falling:
            SET_MASK(EICRA, BIT(ISC01));
            break;
        case Edge_Rising:
            SET_MASK(EICRA, BIT(ISC01) | BIT(ISC00));
            break;
        default:
            HALT; // Should not get here
    }

    SET_BIT(EIMSK, INT0);

    return Status_OK;
}

Std_ReturnType Port_EnableInt1(Edge edge, Callback cbk)
{
    Callback_INT1 = cbk;

    switch(edge)
    {
        case Edge_LowLevel:
            SET_MASK(EICRA, 0);
            break;
        case Edge_Both:
            SET_MASK(EICRA, BIT(ISC10));
            break;
        case Edge_Falling:
            SET_MASK(EICRA, BIT(ISC11));
            break;
        case Edge_Rising:
            SET_MASK(EICRA, BIT(ISC11) | BIT(ISC10));
            break;
        default:
            HALT; // Should not get here
    }

    SET_BIT(EIMSK, INT1);

    return Status_OK;
}

Std_ReturnType Port_SetDataDirection(Port port, uint8_t direction)
{
    Std_ReturnType status = Status_Not_OK;

    if (port < NbrOfPorts )
    {
        /* Write DDRx */
        WRITE_PU8(port + OFFSET_DDR, direction);
        status = Status_OK;
    }

    return status;
}

Std_ReturnType Port_GetDataDirection(Port port, uint8_t* direction)
{
    Std_ReturnType status = Status_Not_OK;

    if ( port < NbrOfPorts )
    {
        /* Read DDRx */
        *direction = READ_PU8(port + OFFSET_DDR);
        status = Status_OK;
    }

    return status;
}

Std_ReturnType Port_SetValue(Port port, uint8_t portValue)
{
    Std_ReturnType status = Status_Not_OK;

    if ( port < NbrOfPorts )
    {
        /* Write PORTx */
        WRITE_PU8(port + OFFSET_PORT, portValue);
        status = Status_OK;
    }

    return status;
}

Std_ReturnType Port_GetValue(Port port, uint8_t* portValue)
{
    Std_ReturnType status = Status_Not_OK;

    if ( port < NbrOfPorts )
    {
        /* Read PINx */
        *portValue = READ_PU8(port + OFFSET_PIN);
        status = Status_OK;
    }

    return status;
}

Std_ReturnType Port_SetPinDataDirection(PinDef pinDef, DataDirection direction)
{
    Std_ReturnType status = Status_Not_OK;
    uint8_t portValue;

    if (pinDef.port < NbrOfPorts && pinDef.pin < NbrOfPins )
    {
        /* Read DDRx */
        status = Port_GetDataDirection(pinDef.port, &portValue);

        if (status == Status_OK)
        {
            switch (direction)
            {
                case Input:
                    RESET_BIT(portValue, pinDef.pin);
                    status = Status_OK;
                    break;
                case Output:
                    SET_BIT(portValue, pinDef.pin);
                    status = Status_OK;
                    break;
                default:
                    status = Status_Not_OK;
                    break;
            }
        }

        if (status == Status_OK)
        {
            status = Port_SetDataDirection(pinDef.port, portValue);
        }
    }

    return status;
}

Std_ReturnType Port_SetPinState(PinDef pinDef, PinState pinValue)
{
    Std_ReturnType status = Status_Not_OK;
    uint8_t portValue;

    if ( pinDef.port < NbrOfPorts && pinDef.pin < NbrOfPins )
    {
        status = Port_GetValue(pinDef.port, &portValue);

        if (status == Status_OK)
        {
            switch(pinValue)
            {
                case Low:
                    RESET_BIT(portValue, pinDef.pin);
                    status = Status_OK;
                    break;
                case High:
                    SET_BIT(portValue, pinDef.pin);
                    status = Status_OK;
                    break;
                default:
                    status = Status_Not_OK;
                    break;
            }
        }

        if (status == Status_OK)
        {
            status = Port_SetValue(pinDef.port, portValue);
        }
    }

    return status;
}

Std_ReturnType Port_GetPinState(PinDef pinDef, PinState *pinValue)
{
    Std_ReturnType status = Status_Not_OK;
    uint8_t portValue;

    if ( pinDef.port < NbrOfPorts && pinDef.pin < NbrOfPins )
    {
        status = Port_GetValue(pinDef.port, &portValue);
        if ( status == Status_OK )
        {
            if (IS_SET_BIT(portValue, pinDef.pin))
            {
                *pinValue = High;
            }
            else
            {
                *pinValue = Low;
            }
        }
    }

    return status;
}

