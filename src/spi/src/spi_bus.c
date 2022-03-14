#include "spi.h"
#include "spi_ll.h"
#include "bits.h"
#include "os_cfg.h"
#include "os_mem.h"

#if OS_MALLOC

spi_bus_t spi_bus_create(spi_t bus)
{
    spi_bus_t self = os_malloc(sizeof(*self));

    if (self == NULL_PTR)
        return NULL_PTR;

    spi_bus_init(self, bus);

    return self;
}

void spi_bus_destroy(spi_bus_t dev)
{
    if (dev != NULL_PTR)
        os_free(dev);
}

#endif // OS_MALLOC

int spi_bus_init(spi_bus_t self, spi_t bus)
{
    spi_ll_init(bus);

    self->bus = bus;
    self->clk = NUMBER_OF_SPI_CLOCK_DIVS;
    self->mode = NUMBER_OF_SPI_MODES;

    return SPI_OK;
}

int spi_bus_configure(spi_bus_t self, spi_clock_t clock, spi_mode_t mode)
{
    spi_ll_set_double_speed(self->bus, FALSE);
    spi_ll_set_clock_polarity(self->bus, FALSE);
    spi_ll_set_clock_phase(self->bus, FALSE);

    switch(clock)
    {
        case SPI_CLOCK_DIV_2:
            spi_ll_set_double_speed(self->bus, TRUE);
        case SPI_CLOCK_DIV_4:
            spi_ll_set_prescaler(self->bus, SPI_LL_PSCL_4);
            break;

        case SPI_CLOCK_DIV_8:
            spi_ll_set_double_speed(self->bus, TRUE);
        case SPI_CLOCK_DIV_16:
            spi_ll_set_prescaler(self->bus, SPI_LL_PSCL_16);
            break;

        case SPI_CLOCK_DIV_32:
            spi_ll_set_double_speed(self->bus, TRUE);
        case SPI_CLOCK_DIV_64:
            spi_ll_set_prescaler(self->bus, SPI_LL_PSCL_64);
            break;

        case SPI_CLOCK_DIV_128:
            spi_ll_set_prescaler(self->bus, SPI_LL_PSCL_128);
            break;

        default:
            return -SPI_ERROR_PRESCALER;
    }

    switch(mode)
    {
        case SPI_MODE_0:
            // Nothing to do
            break;

        case SPI_MODE_1:
            spi_ll_set_clock_phase(self->bus, TRUE);
            break;

        case SPI_MODE_2:
            spi_ll_set_clock_polarity(self->bus, TRUE);
            break;

        case SPI_MODE_3:
            spi_ll_set_clock_polarity(self->bus, TRUE);
            spi_ll_set_clock_phase(self->bus, TRUE);
            break;

        default:
            return -SPI_ERROR_POLARITY;
    }

    return SPI_OK;
}

int spi_bus_read_bytes(spi_bus_t self, void *buffer, int len)
{
    uint8_t *bytes = buffer;

    for (int i = 0 ; i < len ; i++)
    {
        spi_ll_write_byte(self->bus, 0);
        spi_ll_wait_tx(self->bus);
        bytes[i] = spi_ll_read_byte(self->bus);
    }

    return len;
}

int spi_bus_read_byte(spi_bus_t self, uint8_t *byte)
{
    spi_ll_write_byte(self->bus, 0);
    spi_ll_wait_tx(self->bus);
    *byte = spi_ll_read_byte(self->bus);

    return 1;
}

int spi_bus_write_bytes(spi_bus_t self, void *buffer, int len)
{
    uint8_t *bytes = buffer;

    for (int i = 0 ; i < len ; i++)
    {
        spi_ll_write_byte(self->bus, bytes[i]);
        spi_ll_wait_tx(self->bus);
        bytes[i] = spi_ll_read_byte(self->bus);
    }

    return len;
}

int spi_bus_write_byte(spi_bus_t self, uint8_t byte, uint8_t *read)
{
    spi_ll_write_byte(self->bus, byte);
    spi_ll_wait_tx(self->bus);
    if (read != NULL_PTR) *read = spi_ll_read_byte(self->bus);

    return 1;
}

int spi_bus_write_fast(spi_bus_t self, const uint8_t byte)
{
    spi_ll_write_byte(self->bus, byte);
    spi_ll_wait_tx(self->bus);

    return 1;
}
