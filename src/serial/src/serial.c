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
    char         rx_buf[SERIAL_RECEIVE_BUFFER_LENGTH];
    unsigned int rx_sz;
#endif

#if SERIAL_ASYNC_TX != OFF
    volatile const void  *tx_buf;
    volatile unsigned int tx_sz;
#endif

};

/*
 * Private functions prototypes
 */

static void serial_ll_wait_for_tx_ready(serial_bus_t bus);
static void serial_ll_wait_for_rx_ready(serial_bus_t bus);
static int serial_ll_write_byte(serial_bus_t bus, uint8_t byte);
static int serial_ll_read_byte(serial_bus_t bus, uint8_t *byte);

/*
 * Private data
 */

#if SERIAL_ASYNC_RX != OFF || SERIAL_ASYNC_TX != OFF
static struct serial_prv_s instances[NUMBER_OF_SERIAL_BUSES];
#endif // SERIAL_ASYNC_RX != OFF || SERIAL_ASYNC_TX != OFF

/*
 * Public functions
 */

int serial_bus_init(serial_bus_t bus, uint32_t baudrate)
{
    if (bus != SERIAL_BUS_0)
        return -1;

#if SERIAL_ASYNC_RX != OFF
    instances[bus].rx_sz = 0;
#endif

#if SERIAL_ASYNC_TX != OFF
    instances[bus].tx_buf = NULL_PTR;
    instances[bus].tx_sz = 0;
#endif

    // Enable peripheral
    RESET_BIT(PRR, PRUSART0);

    // Set UBRR. Formula is UBRR = (Freq / (BAUD * 16) - 1)
    UBRR0 = (F_CPU/16/baudrate)-1U;

    // Frame format: 8 bits, no parity bit, 1 stop bit
    UCSR0C = 0x06;

    // Enable transmitter
    uint8_t ucsrb = BIT(RXEN0) | BIT(TXEN0);

    // Enable interrupts
#if SERIAL_ASYNC_RX != OFF
    SET_BIT(ucsrb, RXCIE0);
#endif

#if SERIAL_ASYNC_TX != OFF
    SET_BIT(ucsrb, TXCIE0);
#endif

    UCSR0B = ucsrb;

    return 0;
}

#if SERIAL_ASYNC_RX != OFF
ISR(USART_RX_vect)
{
    struct serial_prv_s *ser = &instances[SERIAL_BUS_0];

    uint8_t data = UDR0;
    if (data == SERIAL_LINE_TERMINATOR)
    {
        // Terminate string
        ser->rx_buf[ser->rx_sz++] = 0;

        // Call user callback
        serial_rx_callback(SERIAL_BUS_0, ser->rx_buf, ser->rx_sz);

        // Reset buffer
        ser->rx_sz = 0;
    }
    else
    {
        // TODO : handle buffer full
        ser->rx_buf[ser->rx_sz++] = data;
    }
}
#endif

#if SERIAL_ASYNC_TX != OFF
ISR(USART_TX_vect)
{
    struct serial_prv_s *ser = &instances[SERIAL_BUS_0];

    if (ser->tx_sz > 0)
    {
        uint8_t byte = READ_PU8(ser->tx_buf++);
        serial_ll_write_byte(SERIAL_BUS_0, byte);
        ser->tx_sz--;
    }
    else
    {
        ser->tx_buf = NULL_PTR;
        ser->tx_sz = 0;
    }
}

bool serial_tx_ready(serial_bus_t bus)
{
    return instances[bus].tx_buf == NULL_PTR && instances[bus].tx_sz == 0;
}

void serial_write_async(serial_bus_t bus, const void *buffer, unsigned int length)
{
    if (buffer != NULL_PTR)
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
    serial_ll_wait_for_tx_ready(bus);
    return serial_ll_write_byte(bus, chr);
}

int serial_write_bytes(serial_bus_t bus, const void *buffer, unsigned int length)
{
    int written = 0;
    while(length-- > 0) written += serial_write_byte(bus, READ_PU8(buffer++));
    return written;
}

int serial_new_line(serial_bus_t bus)
{
    static const uint8_t data[2] = {'\r', '\n'};
    return serial_write_bytes(bus, data, 2);
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

int serial_print_P(serial_bus_t bus, const __flash char * string)
{
    int written = 0;
    while(*string != 0) written += serial_write_byte(bus, *string++);
    return written;
}

int serial_println_P(serial_bus_t bus, const __flash char * string)
{
    return serial_print_P(bus, string) + serial_new_line(bus);
}

int serial_read_byte(serial_bus_t bus, uint8_t *byte)
{
    serial_ll_wait_for_rx_ready(bus);
    return serial_ll_read_byte(bus, byte);
}

int serial_read_bytes(serial_bus_t bus, void *buffer, unsigned int length)
{
    int received = 0;

    for(unsigned int i = 0 ; i < length; i++)
    {
        received += serial_read_byte(bus, UINT8_PTR(buffer+i) );
    }

    return received;
}

/*
 * Private functions
 */

static void serial_ll_wait_for_tx_ready(serial_bus_t bus)
{
    UNUSED(bus);
    while (!IS_SET_BIT(UCSR0A, UDRE0));
}

static void serial_ll_wait_for_rx_ready(serial_bus_t bus)
{
    UNUSED(bus);
    while(!IS_SET_BIT(UCSR0A, RXC0));
}

static int serial_ll_read_byte(serial_bus_t bus, uint8_t *byte)
{
    if(bus != SERIAL_BUS_0)
        return 0;

    *byte = UDR0;
    return 1;
}

static int serial_ll_write_byte(serial_bus_t bus, uint8_t byte)
{
    if(bus != SERIAL_BUS_0)
        return 0;

    UDR0 = byte;
    return 1;
}
