#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial_prv.h"
#include "serial_cfg.h"
#include "os_cfg.h"
#include "bits.h"
#include "types.h"

typedef struct {
    serial_bus_t handle;

#if SERIAL_ASYNC_RX != OFF
    serial_rx_callback rx_cbk;
    char rx_buf[SERIAL_RECEIVE_BUFFER_LENGTH];
    char *rx_ptr;
    int rx_sz;
#endif

#if SERIAL_ASYNC_TX != OFF
    volatile void *tx_buf;
    volatile int tx_sz;
#endif

} serial_t;

static serial_t buses[NUMBER_OF_SERIAL_BUSES];

#if SERIAL_ASYNC_RX != OFF
ISR(USART_RX_vect)
{
    char b = serial_hal_read_byte(SERIAL_BUS_0);

    if (b == SERIAL_LINE_TERMINATOR)
    {
        // Terminate string
        *buses[SERIAL_BUS_0].rx_ptr = 0;

        // Call user callback
        if (buses[SERIAL_BUS_0].rx_cbk != NULL_PTR)
        {
            buses[SERIAL_BUS_0].rx_cbk(SERIAL_BUS_0, buses[SERIAL_BUS_0].rx_buf, buses[SERIAL_BUS_0].rx_sz + 1);
        }

        // Reset buffer
        buses[SERIAL_BUS_0].rx_ptr = buses[SERIAL_BUS_0].rx_buf;
        buses[SERIAL_BUS_0].rx_sz = 0;
    }
    else
    {
        // TODO : handle buffer full
        *buses[SERIAL_BUS_0].rx_ptr = b;
        buses[SERIAL_BUS_0].rx_ptr++;
        buses[SERIAL_BUS_0].rx_sz++;
    }
}
#endif

#if SERIAL_ASYNC_TX != OFF
ISR(USART_TX_vect)
{
    if (buses[SERIAL_BUS_0].tx_sz > 0)
    {
        uint8_t b = READ_PU8(buses[SERIAL_BUS_0].tx_buf);
        serial_hal_write_byte(SERIAL_BUS_0, b);
        buses[SERIAL_BUS_0].tx_buf++;
        buses[SERIAL_BUS_0].tx_sz--;
    }
    else
    {
        buses[SERIAL_BUS_0].tx_buf = NULL_PTR;
        buses[SERIAL_BUS_0].tx_sz = 0;
    }
}
#endif

void serial_hal_init(serial_bus_t bus, uint32_t baudrate)
{
    buses[bus].handle = bus;

#if SERIAL_ASYNC_RX != OFF
    buses[bus].rx_cbk = NULL_PTR;
    buses[bus].rx_ptr = buses[bus].rx_buf;
    buses[bus].rx_sz = 0;
#endif

#if SERIAL_ASYNC_TX != OFF
    buses[bus].tx_buf = NULL_PTR;
    buses[bus].tx_sz = 0;
#endif

    // Enable peripheral
    RESET_BIT(PRR, PRUSART0);

    // Formula is UBRR = (Freq / (BAUD * 16) - 1)
    word ubrr = { (F_CPU/16/baudrate)-1U };

    // Set UBRR
    UBRR0H = ubrr.bytes[1];
    UBRR0L = ubrr.bytes[0];

    // Frame format: 8 bits, no parity bit, 1 stop bit
    UCSR0C = 0x06;

    // Enable transmitter
    uint8_t ucsr = BIT(RXEN0) | BIT(TXEN0);

    // Enable interrupts
    switch(bus)
    {
        case SERIAL_BUS_0:
            #if SERIAL_ASYNC_RX != OFF
            SET_BIT(ucsr, RXCIE0);
            #endif

            #if SERIAL_ASYNC_TX != OFF
            SET_BIT(ucsr, TXCIE0);
            #endif

            break;

        default:
            break;
    }

    UCSR0B = ucsr;
}

bool serial_hal_tx_ready(serial_bus_t bus)
{
    switch(bus)
    {
        case SERIAL_BUS_0:
            return IS_SET_BIT(UCSR0A, UDRE0);

        default:
            return FALSE;
    }
}

bool serial_hal_rx_ready(serial_bus_t bus)
{
    switch(bus)
    {
        case SERIAL_BUS_0:
            return IS_SET_BIT(UCSR0A, RXC0);

        default:
            return FALSE;
    }
}

uint8_t serial_hal_read_byte(serial_bus_t bus)
{
    switch(bus)
    {
        case SERIAL_BUS_0:
            return UDR0;

        default:
            return 0;
    }
}

void serial_hal_write_byte(serial_bus_t bus, uint8_t byte)
{
    switch(bus)
    {
        case SERIAL_BUS_0:
            UDR0 = byte;
            break;

        default:
            break;
    }
}

#if SERIAL_ASYNC_RX != OFF
void serial_hal_set_rx_callback(serial_bus_t bus, serial_rx_callback cbk)
{
    buses[bus].rx_cbk = cbk;
}
#endif

#if SERIAL_ASYNC_TX != OFF
bool serial_hal_tx_buffer_ready(serial_bus_t bus)
{
    return buses[bus].tx_buf == NULL_PTR && buses[bus].tx_sz == 0;
}

void serial_hal_set_tx_buffer(serial_bus_t bus, void *buffer, int length)
{
    buses[bus].tx_buf = buffer+1;
    buses[bus].tx_sz = length-1;

    // Kickstart transmission
    uint8_t b = READ_PU8(buffer);
    serial_hal_write_byte(SERIAL_BUS_0, b);
}
#endif
