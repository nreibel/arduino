#include "serial_ll.h"
#include "usart.h"

#include <avr/power.h>
#include <avr/interrupt.h>

static void * __context = NULL_PTR;

/*
 * Interrupt handlers
 */

ISR(USART_TX_vect)
{
    serial_tx_irq_handler(USART0, __context);
}

ISR(USART_RX_vect)
{
    serial_rx_irq_handler(USART0, __context);
}

/*
 * Architecture-specific functions
 */

void serial_ll_power_enable(usart_t usart)
{
    UNUSED(usart);
    power_usart0_enable();
}

void serial_ll_set_irq_context(usart_t usart, void * context)
{
    UNUSED(usart);
    __context = context;
}
