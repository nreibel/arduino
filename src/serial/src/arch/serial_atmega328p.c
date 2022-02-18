#include "serial.h"
#include <avr/io.h>

static const struct serial_bus_s BUS0 = {
    .PRRx = &PRR,
    .UCSRxA = &UCSR0A,
    .UCSRxB = &UCSR0B,
    .UCSRxC = &UCSR0C,
    .UDRx = &UDR0,
    .UBRRx = &UBRR0,
    .PRUSARTx = PRUSART0,
    .RXCx = RXC0,
    .UDREx = UDRE0,
    .TXCIEx = TXCIE0,
    .RXCIEx = RXCIE0,
    .RXENx = RXEN0,
    .TXENx = TXEN0,
};

serial_bus_t SERIAL_BUS_0 = &BUS0;
