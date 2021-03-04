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
    serial_hal_isr_rx();
}

void serial_hal_enable_rx_interrupts()
{
    SET_BIT(UCSR0B, RXCIE0);
}
#endif // SERIAL_ASYNC_RX

#if SERIAL_ASYNC_TX == ON
ISR(USART_TX_vect)
{
    serial_hal_isr_tx();
}

void serial_hal_enable_tx_interrupts()
{
    SET_BIT(UCSR0B, TXCIE0);
}
#endif // SERIAL_ASYNC_TX

void serial_hal_init(int baudrate)
{
    // Enable peripheral
    RESET_BIT(PRR, PRUSART0);

    // Formula is UBRR = (Freq / (BAUD * 16) - 1)
    word ubrr = { (F_CPU/16/baudrate)-1U };

    // Set UBRR
    UBRR0H = ubrr.bytes[1];
    UBRR0L = ubrr.bytes[0];

    // Enable transmitter
    UCSR0B = BIT(RXEN0) | BIT(TXEN0);

    // Frame format: 8 bits, no parity bit, 1 stop bit
    UCSR0C = 0x06;
}

bool serial_hal_tx_ready(void)
{
    return IS_SET_BIT(UCSR0A, UDRE0);
}

bool serial_hal_rx_ready(void)
{
    return IS_SET_BIT(UCSR0A, RXC0);
}

uint8_t serial_hal_read_byte()
{
    return UDR0;
}

void serial_hal_write_byte(uint8_t byte)
{
    UDR0 = byte;
}
