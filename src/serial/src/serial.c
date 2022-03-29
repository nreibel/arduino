#include "serial.h"
#include "serial_cfg.h"
#include "serial_ll.h"
#include "types.h"

/*
 * Private data
 */

static struct {

    bool init;

    #if SERIAL_ASYNC_TX
    volatile const void  *tx_buf;
    volatile unsigned int tx_sz;
    #endif

    #if SERIAL_ASYNC_RX
    char rx_buf[SERIAL_RECEIVE_BUFFER_LENGTH];
    unsigned int rx_sz;
    #endif

} instances[NUMBER_OF_USART];

/*
 * Weak interrupt handlers
 */

__attribute((weak))
void serial_rx_callback(usart_t usart, const char *buffer, unsigned int length)
{
    UNUSED(usart);
    UNUSED(buffer);
    UNUSED(length);
}

__attribute((weak))
void serial_rx_overflow(usart_t usart)
{
    UNUSED(usart);
}

/*
 * Public functions
 */

int serial_init(usart_t usart, uint32_t baudrate)
{
    serial_ll_init(usart, baudrate);

#if SERIAL_ASYNC_RX
    instances[usart].rx_sz = 0;
    serial_ll_set_rx_interrupts(usart, TRUE);
#endif

#if SERIAL_ASYNC_TX
    instances[usart].tx_buf = NULL_PTR;
    instances[usart].tx_sz = 0;
    serial_ll_set_tx_interrupts(usart, TRUE);
#endif

    instances[usart].init = TRUE;

    return SERIAL_OK;
}

#if SERIAL_ASYNC_RX

void serial_rx_irq_handler(usart_t usart)
{
    uint8_t data = serial_ll_read(usart);

    if (instances[usart].rx_sz >= SERIAL_RECEIVE_BUFFER_LENGTH)
    {
        serial_rx_overflow(usart);
    }
    else if (data == SERIAL_LINE_TERMINATOR)
    {
        // Terminate string
        instances[usart].rx_buf[instances[usart].rx_sz++] = 0;

        // Call user callback
        serial_rx_callback(usart, instances[usart].rx_buf, instances[usart].rx_sz);

        // Reset buffer
        instances[usart].rx_sz = 0;
    }
    else
    {
        instances[usart].rx_buf[instances[usart].rx_sz++] = data;
    }
}

#endif

#if SERIAL_ASYNC_TX

void serial_tx_irq_handler(usart_t usart)
{
    if (instances[usart].tx_sz > 0)
    {
        uint8_t byte = READ_PU8(instances[usart].tx_buf++);
        serial_ll_write(usart, byte);
        instances[usart].tx_sz--;
    }
    else
    {
        instances[usart].tx_buf = NULL_PTR;
        instances[usart].tx_sz = 0;
    }
}

bool serial_tx_ready(usart_t usart)
{
    if ( usart >= NUMBER_OF_USART || !instances[usart].init ) return FALSE; // TODO
    return instances[usart].tx_buf == NULL_PTR && instances[usart].tx_sz == 0;
}

int serial_write_async(usart_t usart, const void *buffer, unsigned int length)
{
    if (usart >= NUMBER_OF_USART || !instances[usart].init)
        return -SERIAL_ERROR_INSTANCE;

    if (instances[usart].tx_buf != NULL_PTR || instances[usart].tx_sz > 0)
        return -SERIAL_ERROR_BUSY;

    instances[usart].tx_buf = buffer+1;
    instances[usart].tx_sz = length-1;

    // Kickstart transmission
    uint8_t b = READ_PU8(buffer);
    serial_ll_write(usart, b);

    return SERIAL_OK;
}

#endif

int serial_write_byte(usart_t usart, uint8_t chr)
{
    if (usart >= NUMBER_OF_USART || !instances[usart].init)
        return -SERIAL_ERROR_INSTANCE;

    serial_ll_write(usart, chr);
    serial_ll_wait_tx(usart);

    return 1;
}

int serial_write_bytes(usart_t usart, const void *buffer, unsigned int length)
{
    if (usart >= NUMBER_OF_USART || !instances[usart].init)
        return -SERIAL_ERROR_INSTANCE;

    int written = 0;
    uint8_t *bytes = UINT8_PTR(buffer);

    while(length-- > 0) written += serial_write_byte(usart, *bytes++);

    return written;
}

int serial_new_line(usart_t usart)
{
    if (usart >= NUMBER_OF_USART || !instances[usart].init)
        return -SERIAL_ERROR_INSTANCE;

    static const uint8_t data[2] = {'\r', '\n'};
    return serial_write_bytes(usart, data, 2);
}

int serial_print(usart_t usart, const char * string)
{
    if (usart >= NUMBER_OF_USART || !instances[usart].init)
        return -SERIAL_ERROR_INSTANCE;

    int written = 0;

    while(*string != 0) written += serial_write_byte(usart, *string++);

    return written;
}

int serial_println(usart_t usart, const char * string)
{
    if (usart >= NUMBER_OF_USART || !instances[usart].init)
        return -SERIAL_ERROR_INSTANCE;

    return serial_print(usart, string) + serial_new_line(usart);
}

int serial_print_P(usart_t usart, const __flash char * string)
{
    if (usart >= NUMBER_OF_USART || !instances[usart].init)
        return -SERIAL_ERROR_INSTANCE;

    int written = 0;

    while(*string != 0) written += serial_write_byte(usart, *string++);

    return written;
}

int serial_println_P(usart_t usart, const __flash char * string)
{
    if (usart >= NUMBER_OF_USART || !instances[usart].init)
        return -SERIAL_ERROR_INSTANCE;

    return serial_print_P(usart, string) + serial_new_line(usart);
}

int serial_read_byte(usart_t usart, uint8_t *byte)
{
    if (usart >= NUMBER_OF_USART || !instances[usart].init)
        return -SERIAL_ERROR_INSTANCE;

    serial_ll_wait_rx(usart);

    *byte = serial_ll_read(usart);

    return 1;
}

int serial_read_bytes(usart_t usart, void *buffer, unsigned int length)
{
    if (usart >= NUMBER_OF_USART || !instances[usart].init)
        return -SERIAL_ERROR_INSTANCE;

    int received = 0;
    uint8_t *bytes = UINT8_PTR(buffer);

    while(length-- > 0) received += serial_read_byte(usart, bytes++);

    return received;
}
