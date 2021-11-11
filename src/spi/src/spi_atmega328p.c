#include <avr/io.h>
#include "spi.h"
#include "types.h"
#include "bits.h"

void spi_hal_enable()
{
    // Enable peripheral
    RESET_BIT(PRR, PRSPI);
}

void spi_hal_configure(spi_clock_t clock, spi_mode_t mode)
{
    // Enable SPI, Master Mode
    uint8_t spcr = BIT(MSTR) | BIT(SPE);
    uint8_t spsr = 0;

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

bool spi_hal_ready(void)
{
    return IS_SET_BIT(SPSR, SPIF);
}

void spi_hal_write_byte(const uint8_t write)
{
    SPDR = write;
}

uint8_t spi_hal_read_byte(void)
{
    return SPDR;
}
