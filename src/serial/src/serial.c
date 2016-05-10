#include "serial.h"
#include "serial_cfg.h"
#include "serial_prv.h"
#include "avr/io.h"
#include "bits.h"

static TransmitState transmitStateMachine = Serial_Uninitialized;

static char* transmitBuffer = NULL_PTR;
static int   transmitLength = 0;

Std_ReturnType Serial_IsReady(void)
{
	Std_ReturnType retval = Status_Not_OK;

	if (transmitStateMachine == Serial_Ready)
	{
		retval = Status_OK;
	}

	return retval;
}

void Serial_BackgroundTask()
{
	if (transmitStateMachine == Serial_Writing)
	{
		// Check for empty transmit buffer
		if (IS_SET_BIT(UCSR0A, 5))
		{
			UDR0 = *(transmitBuffer++);
			transmitLength--;

			if (transmitLength == 0)
			{
				transmitBuffer = NULL_PTR;
				transmitStateMachine = Serial_Ready;
			}
		}
	}
}

Std_ReturnType Serial_Print ( const char *buffer, int length )
{
	Std_ReturnType retval = Status_Not_OK;

	if(transmitStateMachine == Serial_Ready && length > 0)
	{
		transmitBuffer = (char*) buffer;
		transmitLength = length;
		transmitStateMachine = Serial_Writing;
		retval = Status_Pending;
	}

	return retval;
}

void Serial_Init ( void )
{
	uint16_t ubrr = ((CLOCK_SPEED/16)/BAUD_RATE)-1;

	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) (ubrr & 0xFF);

	UCSR0B = BIT(4) | BIT(3); // Enable receiver and transmitter
	UCSR0C = 0x06; // Frame format: 8 bits, no parity bit, 1 stop bit

	transmitStateMachine = Serial_Ready;
}
