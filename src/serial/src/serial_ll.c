#include "serial_ll.h"
#include "os_cfg.h"

/*
 * Weak interrupt handlers
 */

__attribute((weak))
void serial_tx_irq_handler(usart_t usart, void * context)
{
    UNUSED(usart);
    UNUSED(context);
}

__attribute((weak))
void serial_rx_irq_handler(usart_t usart, void * context)
{
    UNUSED(usart);
    UNUSED(context);
}

/*
 * Public functions
 */

void serial_ll_init(usart_t usart, uint32_t baudrate)
{
    // Reset USART to default values
    serial_ll_reset(usart);

    // Enable peripheral
    serial_ll_power_enable(usart);

    // Set UBRR. Formula is UBRR = (Freq / (BAUD * 16) - 1)
    usart->ubrr = (F_CPU/16/baudrate)-1U;

    // Frame format: 8 bits, no parity bit, 1 stop bit
    usart->ucsrc.bits.ucsz = 0x3;

    // Enable transmitter
    usart->ucsrb.bits.rxen = 1;
    usart->ucsrb.bits.txen = 1;
}

void serial_ll_set_tx_irq(usart_t usart, bool enabled)
{
    usart->ucsrb.bits.txcie = enabled ? 1 : 0;
}

void serial_ll_set_rx_irq(usart_t usart, bool enabled)
{
    usart->ucsrb.bits.rxcie = enabled ? 1 : 0;
}

void serial_ll_wait_tx(usart_t usart)
{
    while (!usart->ucsra.bits.udre);
}

void serial_ll_wait_rx(usart_t usart)
{
    while (!usart->ucsra.bits.rxc);
}

uint8_t serial_ll_read(usart_t usart)
{
    return usart->udr;
}

void serial_ll_write(usart_t usart, uint8_t byte)
{
    usart->udr = byte;
}

void serial_ll_reset(usart_t usart)
{
    usart->ucsra.reg = 0x0;
    usart->ucsrb.reg = 0x0;
    usart->ucsrc.reg = 0x0;
}
