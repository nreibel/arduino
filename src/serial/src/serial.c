#include "serial.h"
#include "serial_cfg.h"
#include "os_cfg.h"
#include "types.h"
#include "bits.h"

/*
 * Private functions prototypes
 */

extern void serial_ll_reset_device(usart_t self);
extern void serial_ll_power_enable(usart_t self);
static void serial_ll_init(usart_t self, uint32_t baudrate);
static void serial_ll_wait_for_tx_ready(usart_t self);
static void serial_ll_wait_for_rx_ready(usart_t self);
static void serial_ll_write_byte(usart_t self, uint8_t byte);
static uint8_t serial_ll_read_byte(usart_t self);

/*
 * Public functions
 */

int serial_bus_init(serial_bus_t bus, uint32_t baudrate)
{
#if SERIAL_ASYNC_RX != OFF
    bus->rx_sz = 0;
#endif

#if SERIAL_ASYNC_TX
    bus->tx_buf = NULL_PTR;
    bus->tx_sz = 0;
#endif

    serial_ll_init(bus->instance, baudrate);

    return 0;
}

#if SERIAL_ASYNC_RX
void serial_rx_irq_handler(serial_bus_t bus)
{
    uint8_t data = serial_ll_read_byte(bus->instance);

    if (data == SERIAL_LINE_TERMINATOR)
    {
        // Terminate string
        bus->rx_buf[bus->rx_sz++] = 0;

        // Call user callback
        serial_rx_callback(bus, bus->rx_buf, bus->rx_sz);

        // Reset buffer
        bus->rx_sz = 0;
    }
    else
    {
        // TODO : handle buffer full
        bus->rx_buf[bus->rx_sz++] = data;
    }
}
#endif

#if SERIAL_ASYNC_TX
void serial_tx_irq_handler(serial_bus_t bus)
{
    if (bus->tx_sz > 0)
    {
        uint8_t byte = READ_PU8(bus->tx_buf++);
        serial_ll_write_byte(bus->instance, byte);
        bus->tx_sz--;
    }
    else
    {
        bus->tx_buf = NULL_PTR;
        bus->tx_sz = 0;
    }
}

bool serial_tx_ready(serial_bus_t bus)
{
    return bus->tx_buf == NULL_PTR && bus->tx_sz == 0;
}

int serial_write_async(serial_bus_t bus, const void *buffer, unsigned int length)
{
    if (bus == NULL_PTR)
        return -1;

    bus->tx_buf = buffer+1;
    bus->tx_sz = length-1;

    // Kickstart transmission
    uint8_t b = READ_PU8(buffer);
    serial_ll_write_byte(bus->instance, b);

    return 0;
}
#endif

int serial_write_byte(serial_bus_t bus, uint8_t chr)
{
    serial_ll_wait_for_tx_ready(bus->instance);
    serial_ll_write_byte(bus->instance, chr);
    return 1;
}

int serial_write_bytes(serial_bus_t bus, const void *buffer, unsigned int length)
{
    int written = 0;
    while(length-- > 0)
    {
        uint8_t byte = READ_PU8(buffer++);
        written += serial_write_byte(bus, byte);
    }

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
    serial_ll_wait_for_rx_ready(bus->instance);
    *byte = serial_ll_read_byte(bus->instance);
    return 1;
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

static void serial_ll_init(usart_t self, uint32_t baudrate)
{
    // Reset USART to default values
    serial_ll_reset_device(self);

    // Enable peripheral
    serial_ll_power_enable(self);

    // Set UBRR. Formula is UBRR = (Freq / (BAUD * 16) - 1)
    self->UBRR = (F_CPU/16/baudrate)-1U;

    // Frame format: 8 bits, no parity bit, 1 stop bit
    self->UCSRC.bits.UCSZ = 0x3;

    // Enable transmitter
    self->UCSRB.bits.RXEN = 1;
    self->UCSRB.bits.TXEN = 1;

    // Enable interrupts
#if SERIAL_ASYNC_RX != OFF
    self->UCSRB.bits.RXCIE = 1;
#endif

#if SERIAL_ASYNC_TX
    self->UCSRB.bits.TXCIE = 1;
#endif
}

static void serial_ll_wait_for_tx_ready(usart_t self)
{
    while (!self->UCSRA.bits.UDRE);
}

static void serial_ll_wait_for_rx_ready(usart_t self)
{
    while (!self->UCSRA.bits.RXC);
}

static uint8_t serial_ll_read_byte(usart_t self)
{
    return self->UDR;
}

static void serial_ll_write_byte(usart_t self, uint8_t byte)
{
    self->UDR = byte;
}

