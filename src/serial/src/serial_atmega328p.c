#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial_cfg.h"
#include "serial_prv.h"
#include "os_cfg.h"
#include "bits.h"
#include "types.h"

#if SERIAL_ASYNC_RX == ON
ISR(USART_RX_vect)
{
    Serial_HAL_ISR_Rx();
}

void Serial_HAL_EnableRxInterrupts()
{
    SET_BIT(UCSR0B, RXCIE0);
}
#endif // SERIAL_ASYNC_RX

#if SERIAL_ASYNC_TX == ON
ISR(USART_TX_vect)
{
    Serial_HAL_ISR_Tx();
}

void Serial_HAL_EnableTxInterrupts()
{
    SET_BIT(UCSR0B, TXCIE0);
}
#endif // SERIAL_ASYNC_TX

void Serial_HAL_Init_HW(void)
{
    // Enable peripheral
    RESET_BIT(PRR, PRUSART0);

    // Formula is UBRR = (Freq / (BAUD * 16) - 1)
    uint16_t ubrr = (F_CPU/16/SERIAL_BAUD_RATE)-1U;

    // Set UBRR
    UBRR0H = HIGH_BYTE(ubrr);
    UBRR0L = LOW_BYTE(ubrr);

    // Enable transmitter
    UCSR0B = BIT(RXEN0) | BIT(TXEN0);

    // Frame format: 8 bits, no parity bit, 1 stop bit
    UCSR0C = 0x06;
}

boolean Serial_HAL_TxIsReady(void)
{
    return IS_SET_BIT(UCSR0A, UDRE0);
}

boolean Serial_HAL_RxIsReady(void)
{
    return IS_SET_BIT(UCSR0A, RXC0);
}

uint8_t Serial_HAL_ReadByte()
{
    return UDR0;
}

void Serial_HAL_WriteByte(uint8_t byte)
{
    UDR0 = byte;
}
