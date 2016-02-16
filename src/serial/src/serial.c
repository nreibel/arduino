/*
 * serial.c
 *
 *  Created on: 16 févr. 2016
 *      Author: reiboul
 */

#include "serial.h"
#include "serial_cfg.h"
#include "avr/io.h"
#include "bits.h"
#include "types.h"

typedef enum {
	Serial_Uninitialized,
	Serial_Ready,
	Serial_Writing
} TransmitState;

static TransmitState transmitStateMachine = Serial_Uninitialized;
static char *transmitBuffer = NULL_PTR;

Std_ReturnType Serial_IsReady()
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
		while ( !IS_SET_BIT(UCSR0A, 5) ); // Wait for empty transmit buffer
		UDR0 = *(transmitBuffer++);

		if (*transmitBuffer == 0)
		{
			transmitBuffer = NULL_PTR;
			transmitStateMachine = Serial_Ready;
		}
	}
}

Std_ReturnType Serial_Print ( const char * buffer )
{
	Std_ReturnType retval = Status_Not_OK;

	if(transmitStateMachine == Serial_Ready)
	{
		transmitBuffer = (char*) buffer;
		transmitStateMachine = Serial_Writing;
		retval = Status_Pending;
	}

	return retval;
}

void Serial_Init()
{
	uint16_t ubrr = ((CLOCK_SPEED/16)/BAUD_RATE)-1;

	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) (ubrr & 0xFF);

	UCSR0B = BIT(4) | BIT(3); // Enable receiver and transmitter
	UCSR0C = 0x06; // Frame format: 8 bits, no parity bit, 1 stop bit

	transmitStateMachine = Serial_Ready;
}
