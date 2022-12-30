#include "serial.h"
#include "types.h"
#include "os_mem.h"
#include <string.h>
#include <stdio.h>

/*
 * Interrupt handlers
 */

void serial_rx_irq_handler(usart_t usart, void * context)
{
    serial_t self = (serial_t) context;

    const uint8_t data = serial_ll_read(usart);

    if (data == self->ter)
    {
        self->rx.buf[self->rx.cnt] = 0;
        self->cbk(self, SERIAL_EVENT_RX_LINE, self->rx.buf, self->rx.cnt);
        self->rx.cnt = 0;
    }
    else if (self->rx.cnt >= self->rx.len-1)
    {
        self->rx.buf[self->rx.cnt] = 0;
        self->cbk(self, SERIAL_EVENT_RX_OVERFLOW, self->rx.buf, self->rx.cnt);
        self->rx.buf[0] = data;
        self->rx.cnt = 1;
    }
    else
    {
        self->rx.buf[self->rx.cnt++] = data;
        self->cbk(self, SERIAL_EVENT_RX_CHAR, &data, 1);
    }
}

void serial_tx_irq_handler(usart_t usart, void * context)
{
    serial_t self = (serial_t) context;

    if (self->tx.cnt < self->tx.len)
    {
        const uint8_t b = self->tx.buf[self->tx.cnt++];
        serial_ll_write(usart, b);
    }
    else
    {
        serial_ll_set_tx_irq(usart, false);
        self->cbk(self, SERIAL_EVENT_TX_COMPLETE, NULL, 0);
        self->tx.bsy = false;
    }
}

/*
 * Public functions
 */

#if OS_MALLOC
serial_t serial_create(void)
{
    serial_t self = os_malloc(sizeof(*self));
    return self;
}

void serial_destroy(serial_t self)
{
    os_free(self);
}
#endif // OS_MALLOC

int serial_init(serial_t self, usart_t usart, uint32_t baudrate)
{
    memset(self, 0, sizeof(*self));

    serial_ll_init(usart, baudrate);
    serial_ll_set_irq_context(usart, self);

    self->dev = usart;

    return SERIAL_OK;
}

int serial_set_line_terminator(serial_t self, char ter)
{
    self->ter = ter;
    return SERIAL_OK;
}

int serial_set_callback(serial_t self, serial_callback callback, void * buffer, unsigned int length)
{
    self->cbk    = callback;
    self->rx.buf = buffer;
    self->rx.len = length;
    self->rx.cnt = 0;

    serial_ll_set_rx_irq(self->dev, callback == NULL_PTR ? false : true);

    return SERIAL_OK;
}

int serial_read_byte(serial_t self, uint8_t * byte)
{
    serial_ll_wait_rx(self->dev);
    *byte = serial_ll_read(self->dev);
    return 1;
}

int serial_read_bytes(serial_t self, void * buffer, unsigned int length)
{
    int received = 0;
    uint8_t * bytes = buffer;

    for (unsigned int i = 0 ; i < length ; i++)
        received += serial_read_byte(self, &bytes[i]);

    return received;
}

int serial_write_byte(serial_t self, uint8_t chr)
{
    serial_ll_write(self->dev, chr);
    serial_ll_wait_tx(self->dev);
    return 1;
}

int serial_write_bytes(serial_t self, const void * buffer, unsigned int length)
{
    int written = 0;
    const uint8_t * bytes = buffer;

    for (unsigned int i = 0 ; i < length ; i++)
        written += serial_write_byte(self, bytes[i]);

    return written;
}

int serial_write_async(serial_t self, const void * buffer, unsigned int length)
{
    if (self->tx.bsy)
        return -SERIAL_ERR_BUSY;

    self->tx.bsy = true;
    self->tx.buf = buffer;
    self->tx.len = length;
    self->tx.cnt = 0;

    serial_ll_set_tx_irq(self->dev, true);

    // Kickstart transmission
    const uint8_t b = self->tx.buf[self->tx.cnt++];
    serial_ll_write(self->dev, b);

    return SERIAL_OK;
}

int serial_print(serial_t self, const char * string)
{
    int written = 0;

    while(*string != 0) written += serial_write_byte(self, *string++);

    return written;
}

int serial_printf(serial_t self, void * buffer, unsigned int length, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, length, fmt, args);
    va_end(args);

    return serial_print(self, buffer);
}

int serial_printf_async(serial_t self, void * buffer, unsigned int length, const char * fmt, ...)
{
    if (self->tx.bsy)
        return -SERIAL_ERR_BUSY;

    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buffer, length, fmt, args);
    va_end(args);

    return serial_write_async(self, buffer, len);
}

int serial_print_P(serial_t self, flstr_t string)
{
    int written = 0;

    while(*string != 0) written += serial_write_byte(self, *string++);

    return written;
}
