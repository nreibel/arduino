#include "serial_ll.h"
#include "serial_cfg.h"

#include <avr/power.h>
#include <avr/interrupt.h>

extern void serial_rx_irq_handler(usart_t bus);
extern void serial_tx_irq_handler(usart_t bus);

const mem_usart_t instances[NUMBER_OF_USART] = {
    [USART0] = TYPECAST(0xC0, const mem_usart_t)
};

/*
 * Interrupt handlers
 */

#if SERIAL_ASYNC_TX
ISR(USART_TX_vect)
{
    serial_tx_irq_handler(USART0);
}
#endif

#if SERIAL_ASYNC_RX
ISR(USART_RX_vect)
{
    serial_rx_irq_handler(USART0);
}
#endif

/*
 * Architecture-specific functions
 */

int serial_ll_power_enable(usart_t usart)
{
    switch(usart)
    {
        case USART0:
            power_usart0_enable();
            break;

        default:
            return -SERIAL_LL_ERROR_INSTANCE;
    }

    return -SERIAL_LL_OK;
}
