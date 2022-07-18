#include "max31855.h"
#include "spi.h"
#include "types.h"
#include "bits.h"

void max31855_device_init(max31855_t *self, gpio_t *cs)
{
    spi_device_init(&self->spi_device, cs, SPI_CLOCK_DIV_2, SPI_MODE_0);
}

bool max31855_get_temperature(max31855_t *self, float *temperature)
{
    uint8_t bytes[2] = {0};
    spi_device_read_bytes(&self->spi_device, bytes, 2);
    if (CHECK_BIT(bytes[1], 0)) return false;

    float ipart = (bytes[0] << 4) + (bytes[1] >> 4);
    float fpart = ((bytes[1] >> 2) & 0x3) / 4.0;
    *temperature = ipart + fpart;

    // Two's complement
    if (bytes[0] >= 0x80) *temperature -= 4096;

    return true;
}

void max31855_get_internal_temperature(max31855_t *self, float *temperature)
{
    uint8_t bytes[4] = {0};
    spi_device_read_bytes(&self->spi_device, bytes, 4);

    float ipart = bytes[2];
    float fpart = HIGH_NIBBLE(bytes[3]) / 16.0;
    *temperature = ipart + fpart;

    // Two's complement
    if (bytes[2] >= 0x80) *temperature -= 256;
}
