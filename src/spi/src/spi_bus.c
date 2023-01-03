#include "spi.h"
#include "spi_ll.h"
#include "bits.h"
#include "macros.h"
#include "os_cfg.h"
#include "os_mem.h"

#include <util/delay.h>
#include <string.h>

#if OS_MALLOC
spi_bus_t spi_bus_create(void)
{
    spi_bus_t self = os_malloc(sizeof(*self));
    return self;
}

void spi_bus_destroy(spi_bus_t dev)
{
    os_free(dev);
}
#endif // OS_MALLOC

int spi_bus_init(spi_bus_t self, spi_t bus)
{
    CHECK_NULL(self);

    memset(self, 0, sizeof(*self));

    spi_ll_init(bus);
    spi_ll_set_master(bus);
    spi_ll_enable(bus);

    self->bus   = bus;
    self->clock = NUMBER_OF_SPI_CLOCK_DIVS;
    self->mode  = NUMBER_OF_SPI_MODES;

    return SPI_OK;
}

int spi_bus_configure(spi_bus_t self, spi_clock_t clock, spi_mode_t mode)
{
    CHECK_NULL(self);

    if (self->clock != clock)
    {
        switch(clock)
        {
            case SPI_CLOCK_DIV_2:
            case SPI_CLOCK_DIV_8:
            case SPI_CLOCK_DIV_32:
                spi_ll_set_double_speed(self->bus, true);
                break;

            case SPI_CLOCK_DIV_4:
            case SPI_CLOCK_DIV_16:
            case SPI_CLOCK_DIV_64:
            case SPI_CLOCK_DIV_128:
                spi_ll_set_double_speed(self->bus, false);
                break;

            default:
                return -SPI_ERROR_PRESCALER;
        }

        switch(clock)
        {
            case SPI_CLOCK_DIV_2:
            case SPI_CLOCK_DIV_4:
                spi_ll_set_prescaler(self->bus, SPI_LL_PSCL_4);
                break;

            case SPI_CLOCK_DIV_8:
            case SPI_CLOCK_DIV_16:
                spi_ll_set_prescaler(self->bus, SPI_LL_PSCL_16);
                break;

            case SPI_CLOCK_DIV_32:
            case SPI_CLOCK_DIV_64:
                spi_ll_set_prescaler(self->bus, SPI_LL_PSCL_64);
                break;

            case SPI_CLOCK_DIV_128:
                spi_ll_set_prescaler(self->bus, SPI_LL_PSCL_128);
                break;

            default:
                return -SPI_ERROR_PRESCALER;
        }

        self->clock = clock;
    }

    if (self->mode != mode)
    {
        switch(mode)
        {
            case SPI_MODE_0:
                spi_ll_set_clock_polarity(self->bus, false);
                spi_ll_set_clock_phase(self->bus, false);
                break;

            case SPI_MODE_1:
                spi_ll_set_clock_polarity(self->bus, false);
                spi_ll_set_clock_phase(self->bus, true);
                break;

            case SPI_MODE_2:
                spi_ll_set_clock_polarity(self->bus, true);
                spi_ll_set_clock_phase(self->bus, false);
                break;

            case SPI_MODE_3:
                spi_ll_set_clock_polarity(self->bus, true);
                spi_ll_set_clock_phase(self->bus, true);
                break;

            default:
                return -SPI_ERROR_POLARITY;
        }

        self->mode = mode;
    }

    return SPI_OK;
}

int spi_bus_transfer(spi_bus_t self, const void * tx, void * rx, unsigned int len)
{
    CHECK_NULL(self);

    spi_ll_transfer(self->bus, tx, rx, len);

    return len;
}

inline void spi_bus_write_fast(spi_bus_t self, const uint8_t byte)
{
    CHECK_NULL(self);

    spi_ll_write_byte(self->bus, byte);
    _delay_us(1);
}
