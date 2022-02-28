#include "serial_ll.h"
#include "serial_cfg.h"

#include <avr/power.h>
#include <avr/interrupt.h>

extern void serial_rx_irq_handler(usart_t bus);
extern void serial_tx_irq_handler(usart_t bus);

const mem_usart_t instances[NUMBER_OF_USART] = {
    [USART1] = TYPECAST(0xC8, const mem_usart_t)
};

/*
 * Interrupt handlers
 */

#if SERIAL_ASYNC_TX
ISR(USART1_TX_vect)
{
    serial_tx_irq_handler(USART1);
}
#endif

#if SERIAL_ASYNC_RX
ISR(USART1_RX_vect)
{
    serial_rx_irq_handler(USART1);
}
#endif

/*
 * Architecture-specific functions
 */

void serial_ll_power_enable(usart_t self)
{
    if (self == USART1)
    {
        power_usart1_enable();
    }
}

void serial_ll_reset_device(usart_t self)
{
    if (self == USART1)
    {
        instances[self]->ucsra.reg = 0x0;
        instances[self]->ucsrb.reg = 0x0;
        instances[self]->ucsrc.reg = 0x0;
        instances[self]->ucsrd.reg = 0x0;
    }
}
