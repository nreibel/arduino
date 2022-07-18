#include "crc.h"
#include "crc_prv.h"
#include "types.h"
#include "bits.h"

void CRC32_Init(uint32_t *crc)
{
    *crc = CRC32_INIT;
}

void CRC32_Final(uint32_t *crc)
{
    *crc ^= CRC32_XOROUT;
}

void CRC32_Update(uint8_t b, uint32_t *crc)
{
    // Move byte into MSB of 32bit CRC
    *crc ^= TYPECAST(b, uint32_t) << 24;

    for (int i = 0; i < 8; i++)
    {
        if ( CHECK_BIT(*crc, 31) )
        {
            *crc = (*crc << 1) ^ CRC32_POLYNOMIAL;
        }
        else
        {
            *crc <<= 1;
        }
    }
}
