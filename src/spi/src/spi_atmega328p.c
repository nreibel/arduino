#include <avr/io.h>
#include "types.h"
#include "bits.h"
#include "spi_cfg.h"
#include "spi_prv.h"

void Spi_Init_HW(void)
{
    // Enable peripheral
    RESET_BIT(PRR, PRSPI);

    // Enable SPI Master Mode, speed is CLK/4
    SPCR = BIT(MSTR) | BIT(SPE) | SPI_CLOCK_SELECT_BITS;
    if (SPI_CLOCK_DIV_2X) SPSR = BIT(SPI2X);
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
