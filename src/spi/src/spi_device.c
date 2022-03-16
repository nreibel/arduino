#include "spi.h"
#include "spi_ll.h"
#include "bits.h"
#include "os_cfg.h"
#include "os_mem.h"

#if OS_MALLOC

spi_device_t spi_device_create(spi_bus_t bus, gpio_t cs, spi_clock_t clock, spi_mode_t mode)
{
    spi_device_t self = os_malloc(sizeof(*self));

    if (self == NULL_PTR)
        return NULL_PTR;

    spi_device_init(self, bus, cs, clock, mode);

    return self;
}

void spi_device_destroy(spi_device_t dev)
{
    if (dev != NULL_PTR)
        os_free(dev);
}

#endif // OS_MALLOC

int spi_device_init(spi_device_t self, spi_bus_t bus, gpio_t cs, spi_clock_t clock, spi_mode_t mode)
{
    UNUSED(bus);

    self->bus = bus;
    self->cs = cs;
    self->clock = clock;
    self->mode = mode;
    self->transaction_mode = FALSE;

    gpio_set_data_direction(cs, GPIO_OUTPUT_ACTIVE_LOW);

    return SPI_OK;
}

int spi_device_set_transaction_mode(spi_device_t self, bool enabled)
{
    self->transaction_mode = enabled;
    return SPI_OK;
}

int spi_device_enable(spi_device_t self)
{
    spi_bus_configure(self->bus, self->clock, self->mode);
    gpio_set(self->cs);
    return SPI_OK;
}

int spi_device_disable(spi_device_t self)
{
    gpio_reset(self->cs);
    return SPI_OK;
}

int spi_device_read_bytes(spi_device_t self, void *buffer, int len)
{
    if (!self->transaction_mode) spi_device_enable(self);
    int ret = spi_bus_read_bytes(self->bus, buffer, len);
    if (!self->transaction_mode) spi_device_disable(self);

    return ret;
}

int spi_device_read_byte(spi_device_t self, uint8_t *byte)
{
    if (!self->transaction_mode) spi_device_enable(self);
    int ret = spi_bus_read_byte(self->bus, byte);
    if (!self->transaction_mode) spi_device_disable(self);

    return ret;
}

int spi_device_write_bytes(spi_device_t self, void *buffer, int len)
{
    if (!self->transaction_mode) spi_device_enable(self);
    int ret = spi_bus_write_bytes(self->bus, buffer, len);
    if (!self->transaction_mode) spi_device_disable(self);

    return ret;
}

int spi_device_write_byte(spi_device_t self, uint8_t byte, uint8_t *read)
{
    if (!self->transaction_mode) spi_device_enable(self);
    int ret = spi_bus_write_byte(self->bus, byte, read);
    if (!self->transaction_mode) spi_device_disable(self);

    return ret;
}

void spi_device_write_fast(spi_device_t self, const uint8_t byte)
{
    spi_bus_write_fast(self->bus, byte);
}
