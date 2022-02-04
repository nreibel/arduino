#include "spi.h"
#include "spi_ll.h"
#include "gpio.h"
#include "bits.h"

void spi_init(spi_bus_t bus)
{
    UNUSED(bus);

    // Enable MISO, MOSI, SCK
    gpio_set_data_direction(MOSI, GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_set_data_direction(MISO, GPIO_INPUT_HIGH_Z);
    gpio_set_data_direction(SCK, GPIO_OUTPUT_ACTIVE_HIGH);

    spi_ll_enable();
}

void spi_device_init(spi_device_t self, spi_bus_t bus, gpio_t cs, spi_clock_t clk, spi_mode_t mode)
{
    UNUSED(bus);

    self->cs = cs;
    self->clk = clk;
    self->mode = mode;
    self->transaction_mode = FALSE;

    gpio_set_data_direction(cs, GPIO_OUTPUT_ACTIVE_LOW);
}

void spi_set_transaction_mode_enabled(spi_device_t self, bool enabled)
{
    self->transaction_mode = enabled;
}

void spi_enable_slave(spi_device_t self)
{
    spi_ll_configure(self->clk, self->mode);
    gpio_set(self->cs);
}

void spi_disable_slave(spi_device_t self)
{
    gpio_reset(self->cs);
}

void spi_read_bytes(spi_device_t self, uint8_t *buffer, int len)
{
    if (!self->transaction_mode) spi_enable_slave(self);

    while(len-- > 0)
    {
        spi_ll_write_byte(0);
        while( !spi_ll_ready() ); // Wait end of transmission
        uint8_t byte = spi_ll_read_byte();
        WRITE_PU8(buffer++, byte);
    }

    if (!self->transaction_mode) spi_disable_slave(self);
}

void spi_read_byte(spi_device_t self, uint8_t *byte)
{
    if (!self->transaction_mode) spi_enable_slave(self);

    spi_ll_write_byte(0);
    while( !spi_ll_ready() ); // Wait end of transmission
    *byte = spi_ll_read_byte();

    if (!self->transaction_mode) spi_disable_slave(self);
}

void spi_write_bytes(spi_device_t self, uint8_t *buffer, int len)
{
    if (!self->transaction_mode) spi_enable_slave(self);

    while(len-- > 0)
    {
        uint8_t byte = READ_PU8(buffer);
        spi_ll_write_byte(byte);
        while( !spi_ll_ready() ); // Wait end of transmission
        byte = spi_ll_read_byte();
        WRITE_PU8(buffer++, byte);
    }

    if (!self->transaction_mode) spi_disable_slave(self);
}

void spi_write_fast(spi_device_t self, const uint8_t *buffer, int len)
{
    if (!self->transaction_mode) spi_enable_slave(self);

    while(len-- > 0)
    {
        while( !spi_ll_ready() );
        spi_ll_write_byte(*buffer++);
    }

    if (!self->transaction_mode) spi_disable_slave(self);
}

void spi_write_byte(spi_device_t self, uint8_t byte, uint8_t *read)
{
    if (!self->transaction_mode) spi_enable_slave(self);

    spi_ll_write_byte(byte);
    while( !spi_ll_ready() ); // Wait end of transmission
    if (read != NULL_PTR) *read = spi_ll_read_byte();

    if (!self->transaction_mode) spi_disable_slave(self);
}
