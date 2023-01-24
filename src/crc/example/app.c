#include "os.h"
#include "usart.h"
#include "serial.h"
#include "crc.h"

static struct crc_prv_s     crc_obj,    *crc    = &crc_obj;
static struct serial_prv_s  serial_obj, *serial = &serial_obj;

enum {
    CRC32,
    CRC32_BZIP2,
    CRC16_CCIT_ZERO,
    CRC16_ARC,
    CRC8,
    CRC8_DARC,
    NUMBER_OF_CRC
};

static uint8_t crc_data[] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 };

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

int os_putc(char chr, FILE *stream)
{
    UNUSED(stream);
    serial_write_byte(serial, chr);
    return chr;
}

void app_init()
{
    int err = 0;
    uint32_t out = 0;

    serial_init(serial, USART0, 19200);

    // Init tasks
    printf(C_RED "Start!" C_END "\r\n");

    for (int i = 0 ; i < NUMBER_OF_CRC ; i++)
    {
        err += crc_init(crc, &crc_cfg[i]);
        err += crc_feed_bytes(crc, crc_data, sizeof(crc_data));
        err += crc_get_result(crc, &out);

        if (err < 0) printf("CRC Error\r\n");
        else
        {
            printf(crc_expected[i] == out ? C_GREEN : C_RED);
            printf("CRC Expected = %8lx, Calculated = %8lx", crc_expected[i], out);
            printf(C_END "\r\n");
        }
    }
}
