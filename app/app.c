#include "stdio.h"
#include "os.h"
#include "usart.h"
#include "serial.h"
#include "i2c.h"
#include "crc.h"

/*
 * Serial data
 */

static volatile bool tx_ready = true;
static uint8_t serial_rx[16];
static uint8_t serial_tx[64];

static struct serial_instance_s serial_data;
static serial_instance_t serial = &serial_data;

/*
 * PMBus data
 */

static uint8_t page = 0;

/*
 * Private prototypes
 */

int task_main(void * data);

/*
 * Implement printf
 */

int os_putc(char chr, FILE *stream)
{
    UNUSED(stream);
    serial_write_byte(serial, chr);
    return chr;
}

/*
 * Implement PMBus
 */

unsigned int pmbus_tx(uint8_t reg, uint8_t * buffer, unsigned int len)
{
    UNUSED(len);

    switch(reg)
    {
        case 0x00:
            buffer[0] = page;
            return 1;

        default:
            return 0;
    }
}

void pmbus_rx(uint8_t reg, const uint8_t * buffer, unsigned int len)
{
    UNUSED(len);

    switch(reg)
    {
        case 0x00:
            page = buffer[0];
            break;

        default:
            break;
    }
}

void i2c_ll_callback(twi_t twi, i2c_event_t event, unsigned int size)
{
    static uint8_t reg = 0;
    static uint8_t rx[8];
    static uint8_t tx[8];

    switch(event)
    {
        case I2C_EVENT_RX_START:
        {
            i2c_ll_set_rx_buffer(twi, &reg, 1);
            break;
        }

        case I2C_EVENT_RX_MORE:
        {
            i2c_ll_set_rx_buffer(twi, rx, sizeof(rx));
            break;
        }

        case I2C_EVENT_RX_COMPLETE:
        {
            pmbus_rx(reg, rx, size);
            break;
        }

        case I2C_EVENT_TX_START:
        {
            unsigned int len = pmbus_tx(reg, tx, sizeof(tx));
            i2c_ll_set_tx_buffer(twi, tx, len);
            break;
        }

        default:
            break;
    }
}

/*
 * Serial callback
 */

void serial_cbk(serial_instance_t self, serial_event_t event, const uint8_t * buffer, unsigned int length)
{
    UNUSED(self);

    switch(event)
    {
        case SERIAL_EVENT_RX_CHAR:
            // write back to user
            serial_write_bytes(serial, buffer, length);
            break;

        case SERIAL_EVENT_RX_LINE:
            serial_printf_async(self, serial_tx, sizeof(serial_tx), "received %d bytes : %s\r\n", length, buffer);
            break;

        case SERIAL_EVENT_RX_OVERFLOW:
            serial_printf(self, serial_tx, sizeof(serial_tx), "overflown with %d bytes : %s\r\n", length, buffer);
            break;

        case SERIAL_EVENT_TX_COMPLETE:
            tx_ready = true;
            break;

        default:
            break;
    }
}

/*
 * App entry point
 */

struct crc_data_s crc_data;
crc_data_t crc = &crc_data;

void app_init()
{
    int err = 0;
    uint32_t out = 0;
    uint8_t bytes[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

    serial_init(serial, USART0, 19200);
    serial_set_line_terminator(serial, 0x0D);
    serial_set_callback(serial, serial_cbk, serial_rx, sizeof(serial_rx));

    i2c_ll_init_slave(TWI0, 0x20);

    // Init tasks
    printf( C_RED "Start!" C_END "\r\n");

    err += crc_init(crc, 32, CRC32_BZIP2_POLYNOMIAL, CRC32_BZIP2_INIT, CRC32_BZIP2_XOROUT);
    err += crc_feed_bytes(crc, bytes, sizeof(bytes));
    err += crc_final(crc, &out);

    if (err < 0) printf("CRC Error\r\n");
    else printf("CRC = %08lx\r\n", out);

    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);
}

/*
 * Main task
 */

int task_main(void * data)
{
    UNUSED(data);

    static int cpt = 0;
    if (cpt++ & 1) printf(C_GREY "Tock" C_END "\r\n");
    else printf(C_GREY "Tick" C_END "\r\n");

    return EOK;
}
