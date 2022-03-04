#include "spi.h"
#include "spi_ll.h"
#include "bits.h"
#include <avr/io.h>
#include <avr/power.h>

void spi_ll_enable()
{
    // Enable peripheral
    power_spi_enable();
}

void spi_ll_configure(spi_clock_t clock, spi_mode_t mode)
{
    static spi_clock_t _clock = NUMBER_OF_SPI_CLOCK_DIV;
    static spi_mode_t  _mode  = NUMBER_OF_SPI_MODE;

    // Clock/Mode already set
    if (clock == _clock && mode == _mode) return;

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

        default:
            // TODO;
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

        default:
            // TODO;
            break;
    }

    SPCR = spcr;
    SPSR = spsr;

    _clock = clock;
    _mode = mode;
}

bool spi_ll_ready(void)
{
    return IS_SET_BIT(SPSR, SPIF);
}

void spi_ll_write_byte(const uint8_t write)
{
    SPDR = write;
}

uint8_t spi_ll_read_byte(void)
{
    return SPDR;
}
