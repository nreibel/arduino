#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial_cfg.h"
#include "serial_prv.h"
#include "os_cfg.h"
#include "bits.h"
#include "types.h"

#if SERIAL_ASYNC_RX == ON
ISR(USART1_RX_vect)
{
    Serial_HAL_ISR_Rx();
}

void Serial_HAL_EnableRxInterrupts()
{
    SET_BIT(UCSR1B, RXCIE1);
}
#endif // SERIAL_ASYNC_RX

#if SERIAL_ASYNC_TX == ON
ISR(USART1_TX_vect)
{
    Serial_HAL_ISR_Tx();
}

void Serial_HAL_EnableTxInterrupts()
{
    SET_BIT(UCSR1B, TXCIE1);
}
#endif // SERIAL_ASYNC_TX

void Serial_HAL_Init_HW(void)
{
    // Enable peripheral
    RESET_BIT(PRR1, PRUSART1);

    // Formula is UBRR = (Freq / (BAUD * 16) - 1)
    uint16_t ubrr = (F_CPU/16/SERIAL_BAUD_RATE)-1U;

    // Set UBRR
    UBRR1H = HIGH_BYTE(ubrr);
    UBRR1L = LOW_BYTE(ubrr);

    // Enable transmitter
    UCSR1B = BIT(RXEN1) | BIT(TXEN1);

    // Frame format: 8 bits, no parity bit, 1 stop bit
    UCSR1C = 0x06;
}

boolean Serial_HAL_TxIsReady(void)
{
    return IS_SET_BIT(UCSR1A, UDRE1);
}

boolean Serial_HAL_RxIsReady(void)
{
    return IS_SET_BIT(UCSR1A, RXC1);
}

uint8_t Serial_HAL_ReadByte()
{
    return UDR1;
}

void Serial_HAL_WriteByte(uint8_t byte)
{
    UDR1 = byte;
}
