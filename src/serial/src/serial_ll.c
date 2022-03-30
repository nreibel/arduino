#include "serial_ll.h"
#include "os_cfg.h"

extern const mem_usart_t instances[NUMBER_OF_USART];

/*
 * Weak interrupt handlers
 */

__attribute((weak))
void serial_tx_irq_handler(usart_t usart)
{
    UNUSED(usart);
}

__attribute((weak))
void serial_rx_irq_handler(usart_t usart)
{
    UNUSED(usart);
}

/*
 * Public functions
 */

void serial_ll_init(usart_t self, uint32_t baudrate)
{
    // Reset USART to default values
    serial_ll_reset(self);

    // Enable peripheral
    serial_ll_power_enable(self);

    // Set UBRR. Formula is UBRR = (Freq / (BAUD * 16) - 1)
    instances[self]->ubrr = (F_CPU/16/baudrate)-1U;

    // Frame format: 8 bits, no parity bit, 1 stop bit
    instances[self]->ucsrc.bits.ucsz = 0x3;

    // Enable transmitter
    instances[self]->ucsrb.bits.rxen = 1;
    instances[self]->ucsrb.bits.txen = 1;
}

void serial_ll_set_tx_irq(usart_t usart, bool enabled)
{
    instances[usart]->ucsrb.bits.txcie = enabled ? 1 : 0;
}

void serial_ll_set_rx_irq(usart_t usart, bool enabled)
{
    instances[usart]->ucsrb.bits.rxcie = enabled ? 1 : 0;
}

void serial_ll_wait_tx(usart_t usart)
{
    while (!instances[usart]->ucsra.bits.udre);
}

void serial_ll_wait_rx(usart_t usart)
{
    while (!instances[usart]->ucsra.bits.rxc);
}

uint8_t serial_ll_read(usart_t usart)
{
    return instances[usart]->udr;
}

void serial_ll_write(usart_t usart, uint8_t byte)
{
    instances[usart]->udr = byte;
}

void serial_ll_reset(usart_t usart)
{
    instances[usart]->ucsra.reg = 0x0;
    instances[usart]->ucsrb.reg = 0x0;
    instances[usart]->ucsrc.reg = 0x0;
}
