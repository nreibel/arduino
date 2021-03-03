#include "spi.h"
#include "spi_hal.h"
#include "gpio.h"

void spi_init()
{
    gpio_t sck, mosi;

    // Enable MOSI, SCK
    gpio_init(&sck, GPIO_SCK);
    gpio_init(&mosi, GPIO_MOSI);

    gpio_set_data_direction(&sck, GPIO_Output);
    gpio_set_data_direction(&mosi, GPIO_Output);

    spi_hal_enable();
}

void spi_device_init(spi_device_t *self, gpio_t *cs, spi_clock_e clk, spi_mode_e mode)
{
    self->cs = cs;
    self->clk = clk;
    self->mode = mode;
    self->transaction_mode = FALSE;

    gpio_set_data_direction(cs, GPIO_Output);
    gpio_set_state(cs, TRUE);
}

void spi_set_transaction_mode_enabled(spi_device_t *self, bool enabled)
{
    self->transaction_mode = enabled;
}

void spi_enable_slave(spi_device_t *self)
{
    spi_hal_configure(self->clk, self->mode);
    gpio_set_state(self->cs, FALSE);
}

void spi_disable_slave(spi_device_t *self)
{
    gpio_set_state(self->cs, TRUE);
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
