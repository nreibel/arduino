#include <avr/io.h>
#include "serial.h"
#include "serial_cfg.h"
#include "os_cfg.h"
#include "serial_prv.h"
#include "bits.h"
#include "utils.h"

static TransmitState transmitStateMachine = Serial_Uninitialized;

static uint8_t *s_txBuffer = NULL_PTR;
static uint8_t s_rxBuffer[SERIAL_RECEIVE_BUFFER_LENGTH];
static int s_txLength = 0, s_rxLength = 0;

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

    switch (transmitStateMachine)
    {
        case Serial_Writing:
            // Check for empty transmit buffer
            if (IS_SET_BIT(UCSR0A, UDRE0))
            {
                if (s_txLength > 0)
                {
                    UDR0 = *(s_txBuffer++);
                    s_txLength--;
                }
                else
                {
                    s_txBuffer = NULL_PTR;
                    s_txLength = 0;
                    transmitStateMachine = Serial_Ready;
                }
            }

            // More processing needed
            retval = Status_Pending;

        case Serial_Ready:
            if (
                (s_rxLength == 0 || s_rxBuffer[s_rxLength-1] != SERIAL_LINE_TERMINATOR) // buffer is empty OR end char is not received
                && IS_SET_BIT(UCSR0A, RXC0)
                && s_rxLength < SERIAL_RECEIVE_BUFFER_LENGTH
            )
            {
                s_rxBuffer[s_rxLength++] = UDR0;
                retval = Status_Pending;
            }
            break;

        default:
            // No further processing
            retval = Status_OK;
            break;
    }

	return retval;
}

Std_ReturnType Serial_Print( const uint8_t *buffer, int length )
{
    Std_ReturnType retval = Status_Not_OK;

	if (transmitStateMachine == Serial_Ready)
	{
        while ( length > 0 )
        {
            UDR0 = *(buffer++);
            length--;
            while(!IS_SET_BIT(UCSR0A, UDRE0));
        }

        retval = Status_OK;
	}

	return retval;
}

Std_ReturnType Serial_Read(uint8_t *buffer, int buffer_len, int *rcvd_len)
{
    Std_ReturnType retval = Status_Not_OK;
    *rcvd_len = 0;

    // Stop when buffer full
    while ( *rcvd_len < buffer_len )
    {
        while ( !IS_SET_BIT(UCSR0A, RXC0) ); // Wait for reception
        buffer[*rcvd_len] = UDR0;
        // Stop if last char is a line break
        if (buffer[*rcvd_len] == SERIAL_LINE_TERMINATOR)
        {
            break;
        }
        else
        {
            (*rcvd_len)++;
        }
        retval = Status_OK; // At least 1 byte has been read
    }

    return retval;
}

Std_ReturnType Serial_DataReady(boolean *ready)
{
    Std_ReturnType retval = Status_Not_OK;

    if (transmitStateMachine == Serial_Ready)
    {
        *ready = FALSE;
        if (s_rxLength > 0 && (s_rxLength >= SERIAL_RECEIVE_BUFFER_LENGTH || s_rxBuffer[s_rxLength-1] == SERIAL_LINE_TERMINATOR))
        {
            *ready = TRUE;
        }

        retval = Status_OK;
    }

    return retval;
}

Std_ReturnType Serial_AsyncRead(uint8_t *buffer, const int buffer_len, int *rcvd_len)
{
    Std_ReturnType retval = Status_Not_OK;

    if (transmitStateMachine == Serial_Ready)
    {
        int to_copy = MIN(buffer_len, s_rxLength);
        __memcpy(s_rxBuffer, buffer, to_copy);

        *rcvd_len = to_copy;

        // Reset buffer
        s_rxBuffer[0] = 0;
        s_rxLength = 0;

        retval = Status_OK;
    }

    return retval;
}

Std_ReturnType Serial_AsyncPrint ( const uint8_t *buffer, int length )
{
	Std_ReturnType retval = Status_Not_OK;

	if(transmitStateMachine == Serial_Ready && length > 0)
	{
		s_txBuffer = (uint8_t*) buffer;
		s_txLength = length;
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
	uint16_t ubrr = (1000000/SERIAL_BAUD_RATE)-1;
#elif defined F_CPU && F_CPU == F_CPU_2MHZ
	uint16_t ubrr = (125000/SERIAL_BAUD_RATE)-1;
#else
    #error F_CPU is not defined
#endif

	// Set UBRR
	UBRR0H = HIGH_BYTE(ubrr);
	UBRR0L = LOW_BYTE(ubrr);

	// Enable transmitter
	SET_BIT(UCSR0B, RXEN0);
	SET_BIT(UCSR0B, TXEN0);

	// Frame format: 8 bits, no parity bit, 1 stop bit
	UCSR0C = 0x06;

	transmitStateMachine = Serial_Ready;
}
