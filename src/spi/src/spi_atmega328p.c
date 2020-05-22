#include <avr/io.h>
#include "types.h"
#include "bits.h"
#include "spi_cfg.h"
#include "spi_prv.h"

void Spi_HAL_Enable()
{
    // Enable peripheral
    RESET_BIT(PRR, PRSPI);
}

void Spi_HAL_Configure(Spi_Clock clock, Spi_Mode mode)
{
    // Enable SPI, Master Mode
    byte spcr = BIT(MSTR) | BIT(SPE);
    byte spsr = 0;

    switch(clock)
    {
        case SPI_CLOCK_DIV_2:
            SET_BIT(spsr, SPI2X);
        case SPI_CLOCK_DIV_4:
            // Do nothing
            break;
        case SPI_CLOCK_DIV_8:
            SET_BIT(spsr, SPI2X);
        case SPI_CLOCK_DIV_16:
            SET_BIT(spcr, SPR0);
            break;
        case SPI_CLOCK_DIV_32:
            SET_BIT(spsr, SPI2X);
        case SPI_CLOCK_DIV_64:
            SET_BIT(spcr, SPR1);
            break;
        case SPI_CLOCK_DIV_128:
            SET_BIT(spcr, SPR0);
            SET_BIT(spcr, SPR1);
            break;
    }

    switch(mode)
    {
        case SPI_MODE_0:
            // Do nothing
            break;
        case SPI_MODE_1:
            SET_BIT(spcr, CPHA);
            break;
        case SPI_MODE_2:
            SET_BIT(spcr, CPOL);
            break;
        case SPI_MODE_3:
            SET_BIT(spcr, CPOL);
            SET_BIT(spcr, CPHA);
            break;
    }

    SPCR = spcr;
    SPSR = spsr;
}

bool Spi_HAL_IsReady(void)
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
