#include "port.h"
#include "os.h"
#include "types.h"
#include "port_cfg.h"

Std_ReturnType Port_SetDataDirection(Port port, uint8_t portValue)
{
	Std_ReturnType status = Not_OK;

	if (port < NbrOfPorts )
	{
		/* Write DDRx */
		WRITE_PU8(Port_Cfg_BaseAddr[port] + 1, portValue);
	}

	return status;
}

Std_ReturnType Port_SetValue(Port port, uint8_t portValue)
{
	Std_ReturnType status = Not_OK;

	if ( port < NbrOfPorts )
	{
		/* Write PORTx */
		WRITE_PU8(Port_Cfg_BaseAddr[port] + 2, portValue);
		status = OK;
	}

	return status;
}

Std_ReturnType Port_GetValue(Port port, uint8_t* portValue)
{
	Std_ReturnType status = Not_OK;

	if ( port < NbrOfPorts )
	{
		/* Read PINx */
		*portValue = READ_PU8(Port_Cfg_BaseAddr[port] + 0);
		status = OK;
	}

	return status;
}

Std_ReturnType Port_SetPinDataDirection(Port port, Pin pin, DataDirection direction)
{
	Std_ReturnType status = Not_OK;
	uint8_t portValue;

	if (port < NbrOfPorts && pin < NbrOfPins )
	{
		/* Read DDRx */
		portValue = READ_PU8(Port_Cfg_BaseAddr[port] + 1);

		switch (direction)
		{
		case Input:
			RESET_BIT(portValue, pin);
			status = OK;
			break;
		case Output:
			SET_BIT(portValue, pin);
			status = OK;
			break;
		default:
			break;
		}

		if (status == OK)
		{
			/* Write DDRx */
			WRITE_PU8(Port_Cfg_BaseAddr[port]+1, portValue);
		}
	}

	return status;
}

Std_ReturnType Port_SetPinValue(Port port, Pin pin, PinValue pinValue)
{
	Std_ReturnType status = Not_OK;
	uint8_t portValue;

	if ( port < NbrOfPorts && pin < NbrOfPins )
	{
		status = Port_GetValue(port, &portValue);
		if (status == OK)
		{
			switch(pinValue)
			{
			case Low:
				RESET_BIT(portValue, pin);
				status = OK;
				break;
			case High:
				SET_BIT(portValue, pin);
				status = OK;
				break;
			default:
				break;
			}

			status = Port_SetValue(port, portValue);
		}
	}

	return status;
}

Std_ReturnType Port_GetPinValue(Port port, Pin pin, PinValue *pinValue)
{
	Std_ReturnType status = Not_OK;
	uint8_t portValue;

	if ( port < NbrOfPorts && pin < NbrOfPins )
	{
		status = Port_GetValue(port, &portValue);
		if ( status == OK )
		{
			if (IS_SET_BIT(portValue, pin))
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

