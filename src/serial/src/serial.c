#include "os.h"
#include "serial.h"
#include "serial_cfg.h"
#include "serial_ll.h"
#include "types.h"

#ifdef SERIAL_WORKER_TASK
static int deferred_task_rx_cbk(void* data);
#endif

/*
 * Private data
 */

#if SERIAL_ASYNC_RX
static volatile struct {
    bool ready;
    char buf[SERIAL_RECEIVE_BUFFER_LENGTH];
    unsigned int sz;
} rx[NUMBER_OF_USART];
#endif // SERIAL_ASYNC_RX

#if SERIAL_ASYNC_TX
static volatile struct {
    const uint8_t *buf;
    unsigned int sz;
} tx[NUMBER_OF_USART];
#endif // SERIAL_ASYNC_TX

/*
 * Weak interrupt handlers
 */

__attribute((weak))
void serial_rx_callback(usart_t usart, volatile const char *buffer, unsigned int length)
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
    rx[usart].sz = 0;
    rx[usart].ready = false;
    serial_ll_set_rx_irq(usart, true);
#endif

#if SERIAL_ASYNC_TX
    tx[usart].buf = NULL_PTR;
    tx[usart].sz = 0;
    serial_ll_set_tx_irq(usart, true);
#endif

#ifdef SERIAL_WORKER_TASK
        os_task_setup(SERIAL_WORKER_TASK, 1, deferred_task_rx_cbk, NULL_PTR);
        os_task_disable(SERIAL_WORKER_TASK);
#endif // SERIAL_WORKER_TASK

    return SERIAL_OK;
}

#if SERIAL_ASYNC_RX

void serial_rx_irq_handler(usart_t usart)
{
    uint8_t data = serial_ll_read(usart);

#if SERIAL_ECHO
    serial_ll_write(usart, data);
    serial_ll_wait_tx(usart);
#endif

    if (data == SERIAL_LINE_TERMINATOR)
    {
        // Terminate string
        rx[usart].buf[rx[usart].sz++] = 0;

#ifdef SERIAL_WORKER_TASK
        // Signal the deferred routine to execute
        rx[usart].ready = true;
        os_task_enable(SERIAL_WORKER_TASK);
#else
        // Call user callback from interrupt context
        serial_rx_callback(usart, rx[usart].buf, rx[usart].sz-1);
        rx[usart].sz = 0;
#endif // SERIAL_WORKER_TASK

    }
    else if ( rx[usart].sz >= SERIAL_RECEIVE_BUFFER_LENGTH )
    {
        serial_rx_overflow(usart);
    }
    else
    {
        rx[usart].buf[rx[usart].sz++] = data;
    }
}

#endif // SERIAL_ASYNC_RX

#if SERIAL_ASYNC_TX

void serial_tx_irq_handler(usart_t usart)
{
    if (tx[usart].sz > 0)
    {
        uint8_t byte = *PU8(tx[usart].buf++);
        serial_ll_write(usart, byte);
        tx[usart].sz--;
    }
    else
    {
        tx[usart].buf = NULL_PTR;
        tx[usart].sz = 0;
    }
}

bool serial_tx_ready(usart_t usart)
{
    if ( usart >= NUMBER_OF_USART ) return false; // TODO
    return tx[usart].buf == NULL_PTR && tx[usart].sz == 0;
}

int serial_write_async(usart_t usart, const void *buffer, unsigned int length)
{
    uint8_t * bytes = buffer;

    if (usart >= NUMBER_OF_USART)
        return -SERIAL_ERROR_INSTANCE;

    if (tx[usart].buf != NULL_PTR || tx[usart].sz > 0)
        return -SERIAL_ERROR_BUSY;

    tx[usart].buf = bytes+1;
    tx[usart].sz = length-1;

    // Kickstart transmission
    uint8_t b = *PU8(bytes);
    serial_ll_write(usart, b);

    return SERIAL_OK;
}

#endif // SERIAL_ASYNC_TX

int serial_write_byte(usart_t usart, uint8_t chr)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_ERROR_INSTANCE;

    serial_ll_write(usart, chr);
    serial_ll_wait_tx(usart);

    return 1;
}

int serial_write_bytes(usart_t usart, const void *buffer, unsigned int length)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_ERROR_INSTANCE;

    int written = 0;
    uint8_t *bytes = PU8(buffer);

    while(length-- > 0) written += serial_write_byte(usart, *bytes++);

    return written;
}

int serial_new_line(usart_t usart)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_ERROR_INSTANCE;

    static const uint8_t data[2] = {'\r', '\n'};
    return serial_write_bytes(usart, data, 2);
}

int serial_print(usart_t usart, const char * string)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_ERROR_INSTANCE;

    int written = 0;

    while(*string != 0) written += serial_write_byte(usart, *string++);

    return written;
}

int serial_println(usart_t usart, const char * string)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_ERROR_INSTANCE;

    return serial_print(usart, string) + serial_new_line(usart);
}

int serial_print_P(usart_t usart, flstr_t string)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_ERROR_INSTANCE;

    int written = 0;

    while(*string != 0) written += serial_write_byte(usart, *string++);

    return written;
}

int serial_println_P(usart_t usart, flstr_t string)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_ERROR_INSTANCE;

    return serial_print_P(usart, string) + serial_new_line(usart);
}

int serial_read_byte(usart_t usart, uint8_t *byte)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_ERROR_INSTANCE;

    serial_ll_wait_rx(usart);

    *byte = serial_ll_read(usart);

    return 1;
}

int serial_read_bytes(usart_t usart, void *buffer, unsigned int length)
{
    if (usart >= NUMBER_OF_USART)
        return -SERIAL_ERROR_INSTANCE;

    int received = 0;
    uint8_t *bytes = PU8(buffer);

    while(length-- > 0) received += serial_read_byte(usart, bytes++);

    return received;
}

/*
 * Private functions
 */

#ifdef SERIAL_WORKER_TASK

static int deferred_task_rx_cbk(void* data)
{
    UNUSED(data);

    for (usart_t u = 0 ; u < NUMBER_OF_USART ; u++)
    {
        if (rx[u].ready)
        {
            serial_rx_callback(u, rx[u].buf, rx[u].sz-1);
            rx[u].ready = false;
            rx[u].sz = 0;
        }
    }
    return -EDONE;
}

#endif
