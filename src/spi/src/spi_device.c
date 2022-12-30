#include "spi.h"
#include "spi_ll.h"
#include "bits.h"
#include "os_cfg.h"
#include "os_mem.h"
#include "macros.h"

#include <string.h>

#if OS_MALLOC
spi_device_t spi_device_create(void)
{
    spi_device_t self = os_malloc(sizeof(*self));
    return self;
}

void spi_device_destroy(spi_device_t dev)
{
    os_free(dev);
}
#endif // OS_MALLOC

int spi_device_init(spi_device_t self, spi_bus_t bus, gpio_t cs, spi_clock_t clock, spi_mode_t mode)
{
    CHECK_NULL(self);

    memset(self, 0, sizeof(*self));

    self->bus   = bus;
    self->cs    = cs;
    self->clock = clock;
    self->mode  = mode;

    gpio_configure(cs, GPIO_OUTPUT_ACTIVE_LOW);

    return SPI_OK;
}

int spi_device_enable(spi_device_t self)
{
    CHECK_NULL(self);

    spi_bus_configure(self->bus, self->clock, self->mode);
    gpio_set(self->cs);
    return SPI_OK;
}

int spi_device_disable(spi_device_t self)
{
    CHECK_NULL(self);

    gpio_reset(self->cs);
    return SPI_OK;
}

int spi_device_read_bytes(spi_device_t self, void * buffer, unsigned int len)
{
    CHECK_NULL(self);

    int ret = SPI_OK;
    unsigned int cnt = 0;

    ret += spi_device_enable(self);
    cnt += spi_bus_read_bytes(self->bus, buffer, len);
    ret += spi_device_disable(self);

    if (ret == SPI_OK && cnt == len)
        return cnt;
    else return -SPI_FAIL;
}

int spi_device_read_byte(spi_device_t self, uint8_t * byte)
{
    CHECK_NULL(self);

    int ret = SPI_OK;
    unsigned int cnt = 0;

    ret += spi_device_enable(self);
    cnt += spi_bus_read_byte(self->bus, byte);
    ret += spi_device_disable(self);

    if (ret == SPI_OK && cnt == 1)
        return cnt;
    else return -SPI_FAIL;
}

int spi_device_transfer_bytes(spi_device_t self, void *buffer, unsigned int len)
{
    CHECK_NULL(self);

    int ret = SPI_OK;
    unsigned int cnt = 0;

    ret += spi_device_enable(self);
    cnt += spi_bus_transfer_bytes(self->bus, buffer, len);
    ret += spi_device_disable(self);

    if (ret == SPI_OK && cnt == len)
        return cnt;
    else return -SPI_FAIL;
}

int spi_device_transfer_byte(spi_device_t self, uint8_t byte, uint8_t *read)
{
    CHECK_NULL(self);

    int ret = SPI_OK;
    unsigned int cnt = 0;

    ret += spi_device_enable(self);
    cnt += spi_bus_transfer_byte(self->bus, byte, read);
    ret += spi_device_disable(self);

    if (ret == SPI_OK && cnt == 1)
        return cnt;
    else return -SPI_FAIL;
}
