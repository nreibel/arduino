#include "types.h"
#include "bits.h"
#include "max7219.h"
#include "max7219_prv.h"
#include "max7219_charset.h"
#include "spi.h"

void MAX7219_Init(Spi_Slave slave)
{
    MAX7219_WriteRegister(slave, MAX7219_MODE_DECODE, 0);
    MAX7219_WriteRegister(slave, MAX7219_MODE_INTENSITY, 0);
    MAX7219_WriteRegister(slave, MAX7219_MODE_SCAN_LIMIT, 7);
    MAX7219_WriteRegister(slave, MAX7219_MODE_TEST, 0);
    MAX7219_WriteRegister(slave, MAX7219_MODE_POWER, 1);
}

void MAX7219_ClearDisplay(Spi_Slave slave)
{
    for (int d = 0 ; d < 8 ; d++)
    {
        MAX7219_SetDigit(slave, d, 0);
    }
}

void MAX7219_SetEnabled(Spi_Slave slave, bool enabled)
{
    MAX7219_WriteRegister(slave, MAX7219_MODE_POWER, enabled ? 1 : 0);
}

void MAX7219_SetDigit(Spi_Slave slave, int digit, uint8_t value)
{
    if (digit < 0 || digit > 7) { /* TODO : error */ }
    else MAX7219_WriteRegister(slave, digit+1, value);
}

void MAX7219_SetBacklight(Spi_Slave slave, uint8_t value)
{
    value = MAP(value, 0, 255, 0, 0xF);
    MAX7219_WriteRegister(slave, MAX7219_MODE_INTENSITY, value);
}

void MAX7219_WriteRegister(Spi_Slave slave, uint8_t reg, uint8_t data)
{
    Spi_EnableSlave(slave);
    Spi_WriteByte(reg, NULL_PTR);
    Spi_WriteByte(data, NULL_PTR);
    Spi_DisableSlave(slave);
}

int MAX7219_InitStringBuffer(void *buffer, char* str, int len)
{
    int pos = 0;

    for (int idx = 0 ; idx < len ; idx++)
    {
        uint8_t chr = str[idx];
        for (int col = 0 ; col < MAX7219_CHARSET_WIDTH ; col++)
        {
            uint8_t data = s_max7219_charset[chr-0x20][col];
            if (data != 0)
            {
                uint8_t b = data << (8-MAX7219_CHARSET_HEIGHT);
                WRITE_PU8(buffer+pos, b);
                pos++;
            }
        }
        pos++;
    }

    return pos;
}
