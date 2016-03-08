#include "port.h"
#include "bits.h"

Std_ReturnType Port_SetDataDirection(Port port, uint8_t direction)
{
	Std_ReturnType status = Status_Not_OK;

	if (port < NbrOfPorts )
	{
		/* Write DDRx */
		WRITE_PU8(Port_Cfg_BaseAddr[port] + 1, direction);
		status = Status_OK;
	}

	return status;
}

Std_ReturnType Port_GetDataDirection(Port port, uint8_t* direction)
{
	Std_ReturnType status = Status_Not_OK;

	if ( port < NbrOfPorts )
	{
		/* Read PINx */
		*direction = READ_PU8(Port_Cfg_BaseAddr[port] + 1);
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
		WRITE_PU8(Port_Cfg_BaseAddr[port] + 2, portValue);
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
		*portValue = READ_PU8(Port_Cfg_BaseAddr[port] + 0);
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
		portValue = READ_PU8(Port_Cfg_BaseAddr[pinDef.port] + 1);

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
			break;
		}

		if (status == Status_OK)
		{
			/* Write DDRx */
			WRITE_PU8(Port_Cfg_BaseAddr[pinDef.port]+1, portValue);
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
				break;
			}

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

