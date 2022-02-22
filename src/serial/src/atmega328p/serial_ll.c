#include "serial.h"
#include "serial_cfg.h"

#include <avr/power.h>
#include <avr/interrupt.h>

extern void serial_rx_irq_handler(serial_bus_t bus);
extern void serial_tx_irq_handler(serial_bus_t bus);

// Memory allocation for bus objects
static struct serial_bus_s __bus[NUMBER_OF_USART] = {
    [0] = { .instance = USART0 },
};

// Handles to serial bus objects
const serial_bus_t SERIAL_BUS[NUMBER_OF_USART] = {
    [0] = &__bus[0],
};

// Interrupt handlers
#if SERIAL_ASYNC_TX
ISR(USART_TX_vect)
{
    serial_tx_irq_handler(SERIAL_BUS[0]);
}
#endif

#if SERIAL_ASYNC_RX
ISR(USART_RX_vect)
{
    serial_rx_irq_handler(SERIAL_BUS[0]);
}
#endif

// Architecture-specific functions
void serial_ll_power_enable(usart_t self)
{
    if (self == USART0)
    {
        power_usart0_enable();
    }
}

void serial_ll_reset_device(usart_t self)
{
    if (self == USART0)
    {
        self->UCSRA.reg = 0x0;
        self->UCSRB.reg = 0x0;
        self->UCSRC.reg = 0x0;
    }
}
