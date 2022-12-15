#include <string.h>
#include "crc.h"
#include "types.h"
#include "bits.h"

static uint32_t reflect(uint32_t a, unsigned int len);

int crc_init(crc_t self, crc_config_s * cfg)
{
    switch(cfg->length)
    {
        case 8:
        case 16:
        case 32:
            break;

        default:
            return -CRC_ERR_PARAM;
    }

    memset(self, 0, sizeof(*self));

    self->crc = cfg->initial_value;
    self->cfg = cfg;

    return CRC_OK;
}

int crc_reset(crc_t self)
{
    if (self->cfg->length == 0)
        return -CRC_ERR_INIT;

    self->crc = self->cfg->initial_value;
    return CRC_OK;
}

int crc_get_result(crc_t self, uint32_t *crc)
{
    if (self->cfg->length == 0)
        return -CRC_ERR_INIT;

    uint32_t res = self->crc ^ self->cfg->final_xor;

    if (self->cfg->reflect_output)
        res = reflect(res, self->cfg->length);

    res &= (1ul << self->cfg->length) - 1;

    *crc = res;

    return CRC_OK;
}

int crc_feed_bytes(crc_t self, const void * data, unsigned int len)
{
    if (self->cfg->length == 0)
        return -CRC_ERR_INIT;

    int res = CRC_OK;
    const uint8_t * bytes = data;

    while(len-- && res == CRC_OK)
        res += crc_feed_byte(self, *bytes++);

    return CRC_OK;
}

int crc_feed_byte(crc_t self, uint8_t b)
{
    if (self->cfg->length == 0)
        return -CRC_ERR_INIT;

    if (self->cfg->reflect_input)
        b = reflect(b, 8);

    // Move byte into upper byte of CRC
    self->crc ^= TYPECAST(b, uint32_t) << (self->cfg->length - 8);

    for (int i = 0; i < 8; i++)
    {
        if ( CHECK_BIT(self->crc, self->cfg->length - 1) )
        {
            self->crc = (self->crc << 1) ^ self->cfg->polynomial;
        }
        else
        {
            self->crc <<= 1;
        }
    }

    return CRC_OK;
}

/*
 * Private functions
 */

static uint32_t reflect(uint32_t a, unsigned int len)
{
    switch(len)
    {
        case 0:
            return 0;

        case 1:
            return a & 0x1;

        default:
        {
            int n = len/2;
            return reflect(a, n) << n | reflect(a >> n, n);
        }
    }
}
