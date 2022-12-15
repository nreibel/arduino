#include <string.h>
#include "crc.h"
#include "types.h"
#include "bits.h"

static uint32_t reflect(uint32_t a, unsigned int len);

int crc_init(crc_data_t self, unsigned int len, uint32_t poly, uint32_t init, uint32_t xorout, bool refin, bool refout)
{
    switch(len)
    {
        case 8:
        case 16:
        case 32:
            break;

        default:
            return -CRC_ERR_PARAM;
    }

    memset(self, 0, sizeof(*self));

    self->crc       = init;
    self->len       = len;
    self->init      = init;
    self->poly      = poly;
    self->xorout    = xorout;
    self->refin     = refin;
    self->refout    = refout;

    return CRC_OK;
}

int crc_reset(crc_data_t self)
{
    if (self->len == 0)
        return -CRC_ERR_INIT;

    self->crc = self->init;
    return CRC_OK;
}

int crc_final(crc_data_t self, uint32_t *crc)
{
    if (self->len == 0)
        return -CRC_ERR_INIT;

    uint32_t res = self->crc ^ self->xorout;

    if (self->refout)
        res = reflect(res, self->len);

    res &= (1ul << self->len) - 1;

    *crc = res;

    return CRC_OK;
}

int crc_feed_bytes (crc_data_t self, const void * data, unsigned int len)
{
    if (self->len == 0)
        return -CRC_ERR_INIT;

    const uint8_t * bytes = data;

    for (unsigned int i = 0 ; i < len ; i++)
    {
        crc_feed_byte(self, bytes[i]);
    }

    return CRC_OK;
}

int crc_feed_byte(crc_data_t self, uint8_t b)
{
    if (self->len == 0)
        return -CRC_ERR_INIT;

    if (self->refout)
        b = reflect(b, 8);

    // Move byte into upper byte of CRC
    self->crc ^= TYPECAST(b, uint32_t) << (self->len - 8);

    for (int i = 0; i < 8; i++)
    {
        if ( CHECK_BIT(self->crc, self->len - 1) )
        {
            self->crc = (self->crc << 1) ^ self->poly;
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
