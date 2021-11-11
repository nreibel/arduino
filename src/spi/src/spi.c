#include "spi.h"
#include "spi_hal.h"
#include "gpio.h"

void spi_init()
{
    gpio_t sck, mosi, miso;

    // Enable MISO, MOSI, SCK
    gpio_init(&mosi, GPIO_PORT_B, 3, GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_init(&miso, GPIO_PORT_B, 4, GPIO_INPUT_HIGH_Z);
    gpio_init(&sck, GPIO_PORT_B, 5, GPIO_OUTPUT_ACTIVE_HIGH);

    spi_hal_enable();
}

void spi_device_init(spi_device_t *self, gpio_t *cs, spi_clock_t clk, spi_mode_t mode)
{
    self->cs = cs;
    self->clk = clk;
    self->mode = mode;
    self->transaction_mode = FALSE;
}

void spi_set_transaction_mode_enabled(spi_device_t *self, bool enabled)
{
    self->transaction_mode = enabled;
}

void spi_enable_slave(spi_device_t *self)
{
    spi_hal_configure(self->clk, self->mode);
    gpio_set(self->cs);
}

void spi_disable_slave(spi_device_t *self)
{
    gpio_reset(self->cs);
}

void spi_read_bytes(spi_device_t *self, uint8_t *buffer, int len)
{
    if (!self->transaction_mode) spi_enable_slave(self);

    while(len-- > 0)
    {
        spi_hal_write_byte(0);
        while( !spi_hal_ready() ); // Wait end of transmission
        uint8_t byte = spi_hal_read_byte();
        WRITE_PU8(buffer++, byte);
    }

    if (!self->transaction_mode) spi_disable_slave(self);
}

void spi_read_byte(spi_device_t *self, uint8_t *byte)
{
    if (!self->transaction_mode) spi_enable_slave(self);

    spi_hal_write_byte(0);
    while( !spi_hal_ready() ); // Wait end of transmission
    *byte = spi_hal_read_byte();

    if (!self->transaction_mode) spi_disable_slave(self);
}

void spi_write_bytes(spi_device_t *self, uint8_t *buffer, int len)
{
    if (!self->transaction_mode) spi_enable_slave(self);

    while(len-- > 0)
    {
        uint8_t byte = READ_PU8(buffer);
        spi_hal_write_byte(byte);
        while( !spi_hal_ready() ); // Wait end of transmission
        byte = spi_hal_read_byte();
        WRITE_PU8(buffer++, byte);
    }

    if (!self->transaction_mode) spi_disable_slave(self);
}

void spi_write_byte(spi_device_t *self, uint8_t byte, uint8_t *read)
{
    if (!self->transaction_mode) spi_enable_slave(self);

    spi_hal_write_byte(byte);
    while( !spi_hal_ready() ); // Wait end of transmission
    if (read != NULL_PTR) *read = spi_hal_read_byte();

    if (!self->transaction_mode) spi_disable_slave(self);
}
