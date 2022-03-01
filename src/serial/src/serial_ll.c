#include "serial_ll.h"
#include "serial_cfg.h"
#include "os_cfg.h"

extern const mem_usart_t instances[NUMBER_OF_USART];

int serial_ll_init(usart_t self, uint32_t baudrate)
{
    // Reset USART to default values
    serial_ll_reset_device(self);

    // Enable peripheral
    serial_ll_power_enable(self);

    // Set UBRR. Formula is UBRR = (Freq / (BAUD * 16) - 1)
    instances[self]->ubrr = (F_CPU/16/baudrate)-1U;

    // Frame format: 8 bits, no parity bit, 1 stop bit
    instances[self]->ucsrc.bits.ucsz = 0x3;

    // Enable transmitter
    instances[self]->ucsrb.bits.rxen = 1;
    instances[self]->ucsrb.bits.txen = 1;

    // Enable interrupts
#if SERIAL_ASYNC_RX
    instances[self]->ucsrb.bits.rxcie = 1;
#endif

#if SERIAL_ASYNC_TX
    instances[self]->ucsrb.bits.txcie = 1;
#endif

    return SERIAL_LL_OK;
}

int serial_ll_wait_for_tx_ready(usart_t usart)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_LL_ERROR_INSTANCE;

    while (!instances[usart]->ucsra.bits.udre);

    return SERIAL_LL_OK;
}

int serial_ll_wait_for_rx_ready(usart_t usart)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_LL_ERROR_INSTANCE;

    while (!instances[usart]->ucsra.bits.rxc);

    return SERIAL_LL_OK;
}

int serial_ll_read_byte(usart_t usart, uint8_t *byte)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_LL_ERROR_INSTANCE;

    *byte = instances[usart]->udr;

    return SERIAL_LL_OK;
}

int serial_ll_write_byte(usart_t usart, uint8_t byte)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_LL_ERROR_INSTANCE;

    instances[usart]->udr = byte;

    return SERIAL_LL_OK;
}

int serial_ll_reset_device(usart_t usart)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_LL_ERROR_INSTANCE;

    instances[usart]->ucsra.reg = 0x0;
    instances[usart]->ucsrb.reg = 0x0;
    instances[usart]->ucsrc.reg = 0x0;

    return SERIAL_LL_OK;
}
