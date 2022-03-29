#include "spi_ll.h"
#include "gpio_ll.h"
#include "bits.h"

#include <avr/power.h>

void spi_ll_init(spi_t spi)
{
    // Init GPIO
    gpio_ll_set_data_direction(PORT_B, 3, TRUE);  // MOSI
    gpio_ll_set_data_direction(PORT_B, 4, FALSE); // MISO
    gpio_ll_set_data_direction(PORT_B, 5, TRUE);  // SCK

    // Enable peripheral
    power_spi_enable();

    // Init device
    spi->spcr.reg = 0;;
    spi->spsr.reg = 0;
}

void spi_ll_enable(spi_t spi)
{
    spi->spcr.bits.spe = 1;
}

void spi_ll_disable(spi_t spi)
{
    spi->spcr.bits.spe = 0;
}

void spi_ll_set_master(spi_t spi)
{
    spi->spcr.bits.mstr = 1;
}

void spi_ll_set_slave(spi_t spi)
{
    spi->spcr.bits.mstr = 0;
}

void spi_ll_set_double_speed(spi_t spi, bool dbl)
{
    spi->spsr.bits.spi2x = dbl ? 1 : 0;
}

void spi_ll_set_prescaler(spi_t spi, uint8_t prescaler)
{
    spi->spcr.bits.spr = prescaler;
}

void spi_ll_set_clock_polarity(spi_t spi, bool polarity)
{
    spi->spcr.bits.cpol = polarity ? 1 : 0;
}

void spi_ll_set_clock_phase(spi_t spi, bool phase)
{
    spi->spcr.bits.cpha = phase ? 1 : 0;
}

bool spi_ll_ready(spi_t spi)
{
    return spi->spsr.bits.spif;
}

void spi_ll_wait_tx(spi_t spi)
{
    while(!spi->spsr.bits.spif);
}

void spi_ll_write_byte(spi_t spi, const uint8_t write)
{
    spi->spdr = write;
}

uint8_t spi_ll_read_byte(spi_t spi)
{
    return spi->spdr;
}
