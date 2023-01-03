#include "spi_ll.h"
#include "gpio_ll.h"
#include "bits.h"

#include <avr/power.h>
#include <avr/interrupt.h>

static volatile struct {
    const uint8_t * tx;
    uint8_t *       rx;
    unsigned int    length;
    bool            complete;
} xfer_data;

ISR(SPI_STC_vect)
{
    if (xfer_data.rx != NULL_PTR)
        *(xfer_data.rx++) = SPI0->spdr;

    if (xfer_data.length-- > 0)
    {
        SPI0->spdr = *(xfer_data.tx++);
    }
    else
    {
        SPI0->spcr.bits.spie = false;
        xfer_data.complete = true;
    }
}

void spi_ll_init(spi_t spi)
{
    // Init GPIO
    gpio_ll_set_data_direction(PORT_B, 3, true);  // MOSI
    gpio_ll_set_data_direction(PORT_B, 4, false); // MISO
    gpio_ll_set_data_direction(PORT_B, 5, true);  // SCK

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

void spi_ll_write_byte(spi_t spi, const uint8_t write)
{
    spi->spdr = write;
}

uint8_t spi_ll_read_byte(spi_t spi)
{
    return spi->spdr;
}

void spi_ll_transfer(spi_t spi, const uint8_t * tx, uint8_t * rx, unsigned int len)
{
    xfer_data.tx = tx;
    xfer_data.rx = rx;
    xfer_data.length = len;
    xfer_data.complete = false;

    // Start transmission
    spi->spcr.bits.spie = true;
    spi->spdr = *(xfer_data.tx++);

    while(!xfer_data.complete);
}
