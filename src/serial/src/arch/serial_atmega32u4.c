#include "serial.h"
#include <avr/io.h>

static const struct serial_bus_s BUS0 = {
    .PRRx = &PRR1,
    .UCSRxA = &UCSR1A,
    .UCSRxB = &UCSR1B,
    .UCSRxC = &UCSR1C,
    .UDRx = &UDR1,
    .UBRRx = &UBRR1,
    .PRUSARTx = PRUSART1,
    .RXCx = RXC1,
    .UDREx = UDRE1,
    .TXCIEx = TXCIE1,
    .RXCIEx = RXCIE1,
    .RXENx = RXEN1,
    .TXENx = TXEN1,
};

serial_bus_t SERIAL_BUS_0 = &BUS0;
