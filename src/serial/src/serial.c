#include "serial.h"
#include "serial_cfg.h"
#include "os_cfg.h"
#include "types.h"
#include "bits.h"

#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * Private types
 */

struct serial_prv_s {

#if SERIAL_ASYNC_RX != OFF
    serial_rx_callback rx_cbk;
    char  rx_buf[SERIAL_RECEIVE_BUFFER_LENGTH];
    char *rx_ptr;
    int   rx_sz;
#endif

#if SERIAL_ASYNC_TX != OFF
    volatile void *tx_buf;
    volatile int   tx_sz;
#endif

};

/*
 * Private functions prototypes
 */

static bool serial_ll_tx_ready(serial_bus_t bus);
static bool serial_ll_rx_ready(serial_bus_t bus);
static void serial_ll_write_byte(serial_bus_t bus, uint8_t byte);
static uint8_t serial_ll_read_byte(serial_bus_t bus);

/*
 * Private data
 */

static struct serial_prv_s instances[NUMBER_OF_SERIAL_BUSES];

/*
 * Public functions
 */

void serial_bus_init(serial_bus_t bus, uint32_t baudrate)
{

#if SERIAL_ASYNC_RX != OFF
    instances[bus].rx_cbk = NULL_PTR;
    instances[bus].rx_ptr = instances[bus].rx_buf;
    instances[bus].rx_sz = 0;
#endif

#if SERIAL_ASYNC_TX != OFF
    instances[bus].tx_buf = NULL_PTR;
    instances[bus].tx_sz = 0;
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
#if SERIAL_ASYNC_RX != OFF
    SET_BIT(ucsr, RXCIE0);
#endif

#if SERIAL_ASYNC_TX != OFF
    SET_BIT(ucsr, TXCIE0);
#endif

    UCSR0B = ucsr;
}

#if SERIAL_ASYNC_RX != OFF
ISR(USART_RX_vect)
{
    char b = serial_ll_read_byte(SERIAL_BUS_0);

    if (b == SERIAL_LINE_TERMINATOR)
    {
        // Terminate string
        *instances[SERIAL_BUS_0].rx_ptr = 0;

        // Call user callback
        if (instances[SERIAL_BUS_0].rx_cbk != NULL_PTR)
        {
            instances[SERIAL_BUS_0].rx_cbk(SERIAL_BUS_0, instances[SERIAL_BUS_0].rx_buf, instances[SERIAL_BUS_0].rx_sz + 1);
        }

        // Reset buffer
        instances[SERIAL_BUS_0].rx_ptr = instances[SERIAL_BUS_0].rx_buf;
        instances[SERIAL_BUS_0].rx_sz = 0;
    }
    else
    {
        // TODO : handle buffer full
        *instances[SERIAL_BUS_0].rx_ptr = b;
        instances[SERIAL_BUS_0].rx_ptr++;
        instances[SERIAL_BUS_0].rx_sz++;
    }
}

void serial_set_rx_callback(serial_bus_t bus, serial_rx_callback cbk)
{
    instances[bus].rx_cbk = cbk;
}
#endif

#if SERIAL_ASYNC_TX != OFF
ISR(USART_TX_vect)
{
    if (instances[SERIAL_BUS_0].tx_sz > 0)
    {
        uint8_t b = READ_PU8(instances[SERIAL_BUS_0].tx_buf);
        serial_ll_write_byte(SERIAL_BUS_0, b);

        instances[SERIAL_BUS_0].tx_buf++;
        instances[SERIAL_BUS_0].tx_sz--;
    }
    else
    {
        instances[SERIAL_BUS_0].tx_buf = NULL_PTR;
        instances[SERIAL_BUS_0].tx_sz = 0;
    }
}

bool serial_tx_ready(serial_bus_t bus)
{
    return instances[bus].tx_buf == NULL_PTR && instances[bus].tx_sz == 0;
}

void serial_write_async(serial_bus_t bus, void *buffer, int length)
{
    if (length > 0 && buffer != NULL_PTR)
    {
        instances[bus].tx_buf = buffer+1;
        instances[bus].tx_sz = length-1;

        // Kickstart transmission
        uint8_t b = READ_PU8(buffer);
        serial_ll_write_byte(SERIAL_BUS_0, b);
    }
}
#endif

int serial_write_byte(serial_bus_t bus, uint8_t chr)
{
    serial_ll_write_byte(bus, chr);
    while( !serial_ll_tx_ready(bus) );
    return 1;
}

int serial_write_bytes(serial_bus_t bus, void *buffer, int length)
{
    int written = 0;

    for (int i = 0 ; i < length ; i++)
    {
        written += serial_write_byte(bus, READ_PU8(buffer+i) );
    }

    return written;
}

int serial_new_line(serial_bus_t bus)
{
    return serial_write_bytes(bus, "\r\n", 2);
}

int serial_print(serial_bus_t bus, const char * string)
{
    int written = 0;
    while(*string != 0) written += serial_write_byte(bus, *string++);
    return written;
}

int serial_println(serial_bus_t bus, const char * string)
{
    return serial_print(bus, string) + serial_new_line(bus);
}

int serial_print_p(serial_bus_t bus, const __flash char * string)
{
    int written = 0;
    while(*string != 0) written += serial_write_byte(bus, *string++);
    return written;
}

int serial_println_p(serial_bus_t bus, const __flash char * string)
{
    return serial_print_p(bus, string) + serial_new_line(bus);
}

int serial_read_byte(serial_bus_t bus, uint8_t *byte)
{
    while( !serial_ll_rx_ready(bus) );
    *byte = serial_ll_read_byte(bus);
    return 1;
}

int serial_read_bytes(serial_bus_t bus, void *buffer, int length)
{
    int received = 0;

    for(int i = 0 ; i < length; i++)
    {
        received += serial_read_byte(bus, UINT8_PTR(buffer+i) );
    }

    return received;
}

/*
 * Private functions
 */

static bool serial_ll_tx_ready(serial_bus_t bus)
{
    UNUSED(bus);
    return IS_SET_BIT(UCSR0A, UDRE0);
}

static bool serial_ll_rx_ready(serial_bus_t bus)
{
    UNUSED(bus);
    return IS_SET_BIT(UCSR0A, RXC0);
}

static uint8_t serial_ll_read_byte(serial_bus_t bus)
{
    UNUSED(bus);
    return UDR0;
}

static void serial_ll_write_byte(serial_bus_t bus, uint8_t byte)
{
    UNUSED(bus);
    UDR0 = byte;
}
