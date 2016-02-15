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

void Serial_Print ( const char* buffer )
{
	// Send string
	char* ptr = (char*) buffer;
	while ( *ptr != 0 )
	{
		while ( !IS_SET_BIT(UCSR0A, 5) ); // Wait for empty transmit buffer
		UDR0 = *(ptr++);
	}
}

void Serial_Init()
{
	UBRR0H = (uint8) (UBRR >> 8);
	UBRR0L = (uint8) (UBRR & 0xFF);

	UCSR0B = BIT(4) | BIT(3); // Enable receiver and transmitter
	UCSR0C = 0x06; // Frame format: 8 bits, no parity bit, 1 stop bit
}
