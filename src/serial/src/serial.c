#include "serial.h"
#include "serial_cfg.h"
#include "os_cfg.h"
#include "serial_prv.h"
#include "avr/io.h"
#include "bits.h"

static TransmitState transmitStateMachine = Serial_Uninitialized;

static char* transmitBuffer = NULL_PTR;
static int   transmitLength = 0;

boolean Serial_IsReady(void)
{
	boolean retval = FALSE;

	if (transmitStateMachine == Serial_Ready)
	{
		retval = TRUE;
	}

	return retval;
}

Std_ReturnType Serial_BackgroundTask()
{
	Std_ReturnType retval = Status_OK;

	if (transmitStateMachine == Serial_Writing)
	{
		// Check for empty transmit buffer
		if (IS_SET_BIT(UCSR0A, UDRE0))
		{
			if (transmitLength > 0)
			{
				UDR0 = *(transmitBuffer++);
				transmitLength--;
			}
			else
			{
				transmitStateMachine = Serial_Ready;
			}
		}

		// More processing needed
		retval = Status_Pending;
	}

	return retval;
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

void Serial_Init()
{
	// Enable peripheral
	RESET_BIT(PRR, PRUSART0);

    // Formula is UBRR = (Freq / (BAUD * 16) - 1)
#if defined F_CPU && F_CPU == F_CPU_16MHZ
	uint16_t ubrr = (1000000/BAUD_RATE)-1;
#elif defined F_CPU && F_CPU == F_CPU_2MHZ
	uint16_t ubrr = (125000/BAUD_RATE)-1;
#else
    #error F_CPU is not defined
#endif

	// Set UBRR
	UBRR0H = HIGH_BYTE(ubrr);
	UBRR0L = LOW_BYTE(ubrr);

	// Enable transmitter
	SET_BIT(UCSR0B, TXEN0);

	// Frame format: 8 bits, no parity bit, 1 stop bit
	UCSR0C = 0x06;

	transmitStateMachine = Serial_Ready;
}
