#include "serial_ll.h"
#include "serial_tp.h"
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

/*
 * Static storage
 */

static crc_t    crc;
static serial_t serial;

enum {
    CRC32,
    CRC32_BZIP2,
    CRC16_CCIT_ZERO,
    CRC16_ARC,
    CRC8,
    CRC8_DARC,
    NUMBER_OF_CRC
};

static uint8_t crc_data[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

static uint32_t crc_expected[NUMBER_OF_CRC] = {
    [CRC32]             = 0xCBF43926,
    [CRC32_BZIP2]       = 0xFC891918,
    [CRC16_CCIT_ZERO]   = 0x31C3,
    [CRC16_ARC]         = 0xBB3D,
    [CRC8]              = 0xF4,
    [CRC8_DARC]         = 0x15,
};

static crc_config_s crc_cfg[NUMBER_OF_CRC] = {
    [CRC32] = {
        .length         = 32,
        .polynomial     = 0x04C11DB7,
        .initial_value  = 0xFFFFFFFF,
        .final_xor      = 0xFFFFFFFF,
        .reflect_input  = true,
        .reflect_output = true,
    },
    [CRC32_BZIP2] = {
        .length         = 32,
        .polynomial     = 0x04C11DB7,
        .initial_value  = 0xFFFFFFFF,
        .final_xor      = 0xFFFFFFFF,
        .reflect_input  = false,
        .reflect_output = false,
    },
    [CRC16_CCIT_ZERO] = {
        .length         = 16,
        .polynomial     = 0x1021,
        .initial_value  = 0x0,
        .final_xor      = 0x0,
        .reflect_input  = false,
        .reflect_output = false,
    },
    [CRC16_ARC] = {
        .length         = 16,
        .polynomial     = 0x8005,
        .initial_value  = 0x0,
        .final_xor      = 0x0,
        .reflect_input  = true,
        .reflect_output = true,
    },
    [CRC8] = {
        .length         = 8,
        .polynomial     = 0x7,
        .initial_value  = 0x0,
        .final_xor      = 0x0,
        .reflect_input  = false,
        .reflect_output = false,
    },
    [CRC8_DARC] = {
        .length         = 8,
        .polynomial     = 0x39,
        .initial_value  = 0x0,
        .final_xor      = 0x0,
        .reflect_input  = true,
        .reflect_output = true,
    },
};

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

void i2c_ll_callback(const i2c_ll_callback_args_t * args)
{
    static uint8_t reg = 0;
    static uint8_t rx[8];
    static uint8_t tx[8];

    switch(args->event)
    {
        case I2C_EVENT_RX_START:
        {
            i2c_ll_set_rx_buffer(args->twi, &reg, 1);
            break;
        }

        case I2C_EVENT_RX_MORE:
        {
            i2c_ll_set_rx_buffer(args->twi, rx, sizeof(rx));
            break;
        }

        case I2C_EVENT_RX_COMPLETE:
        {
            pmbus_rx(reg, rx, args->size);
            break;
        }

        case I2C_EVENT_TX_START:
        {
            unsigned int len = pmbus_tx(reg, tx, sizeof(tx));
            i2c_ll_set_tx_buffer(args->twi, tx, len);
            break;
        }

        default:
            break;
    }
}

/*
 * Serial callback
 */

void serial_cbk(serial_t self, serial_event_t event, const uint8_t * buffer, unsigned int length)
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

void serial_tp_callback(serial_t bus, const serial_tp_request_t * req, serial_tp_response_t * rsp)
{
    #define FUNCTION_WRITE_DATA   0x10
    #define FUNCTION_READ_DATA    0x11

    #define DATA_X 0x01
    #define DATA_Y 0x02

    UNUSED(bus);

    static uint8_t x = 0x11;
    static uint8_t y = 0x22;

    switch(req->function)
    {
        case FUNCTION_READ_DATA:
        {
            switch(req->address)
            {
                case DATA_X:
                    rsp->data[0] = x;
                    rsp->length = 1;
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;

                case DATA_Y:
                    rsp->data[0] = y;
                    rsp->length = 1;
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;

                default:
                    rsp->status = SERIAL_TP_RETCODE_ADDRESS_INVALID;
                    break;
            }

            break;
        }

        case FUNCTION_WRITE_DATA:
        {
            switch(req->address)
            {
                case DATA_X:
                    x = req->data[0];
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;

                case DATA_Y:
                    y = req->data[0];
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;

                default:
                    rsp->status = SERIAL_TP_RETCODE_ADDRESS_INVALID;
                    break;
            }

            break;
        }

        default:
        {
            rsp->status = SERIAL_TP_RETCODE_FUNCTION_INVALID;
            break;
        }
    }
}

/*
 * App entry point
 */

void app_init()
{
    int err = 0;
    uint32_t out = 0;

    serial = serial_create();
    crc = crc_create();

    serial_init(serial, USART0, 19200);
    serial_set_line_terminator(serial, 0x0D);
    serial_set_callback(serial, serial_cbk, serial_rx, sizeof(serial_rx));

    // serial_tp_init(serial);

    i2c_ll_init_slave(TWI0, 0x20, 0xFF);

    // Init tasks
    printf( C_RED "Start!" C_END "\r\n");

    for (int i = 0 ; i < NUMBER_OF_CRC ; i++)
    {
        err += crc_init(crc, &crc_cfg[i]);
        err += crc_feed_bytes(crc, crc_data, sizeof(crc_data));
        err += crc_get_result(crc, &out);

        if (err < 0) printf("CRC Error\r\n");
        else
        {
            printf(crc_expected[i] == out ? C_GREEN : C_RED);
            printf("CRC Expected = %8lx, Calculated = %8lx\r\n", crc_expected[i], out);
            printf(C_END);
        }
    }

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
