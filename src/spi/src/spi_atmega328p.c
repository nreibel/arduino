#include <avr/io.h>
#include "types.h"
#include "bits.h"

void Spi_Init_HW(void)
{
    // Enable peripheral
    RESET_BIT(PRR, PRSPI);

    // Enable SPI Master Mode, speed is CLK/4
    SPCR = BIT(MSTR) | BIT(SPE);
}

boolean Spi_HAL_IsReady(void)
{
    return IS_SET_BIT(SPSR, SPIF);
}

void Spi_HAL_WriteByte(const uint8_t write)
{
    SPDR = write;
}

uint8_t Spi_HAL_ReadByte(void)
{
    return SPDR;
}
