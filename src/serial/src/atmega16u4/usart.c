#include "serial_ll.h"

#include <avr/power.h>
#include <avr/interrupt.h>

const mem_usart_t instances[NUMBER_OF_USART] = {
    [USART1] = TYPECAST(0xC8, const mem_usart_t)
};

/*
 * Interrupt handlers
 */

ISR(USART1_TX_vect)
{
    serial_tx_irq_handler(USART1);
}

ISR(USART1_RX_vect)
{
    serial_rx_irq_handler(USART1);
}

/*
 * Architecture-specific functions
 */

void serial_ll_power_enable(usart_t usart)
{
    UNUSED(usart);
    power_usart1_enable();
}
