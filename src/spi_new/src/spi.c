#include "spi.h"
#include "spi_hal.h"
#include "gpio.h"

void spi_init()
{
    // Enable MOSI, SCK
    GPIO_SetDataDirection(GPIO_MOSI, GPIO_Output);
    GPIO_SetDataDirection(GPIO_SCK, GPIO_Output);

    spi_hal_enable();
}

void spi_device_init(spi_device_t *self, GPIO cs, spi_clock_e clk, spi_mode_e mode)
{
    self->cs = cs;
    self->clk = clk;
    self->mode = mode;

    GPIO_SetDataDirection(cs, GPIO_Output);
    GPIO_SetState(cs, GPIO_High);
}

void spi_enable_slave(spi_device_t *self)
{
    GPIO_SetState(self->cs, GPIO_Low);
}

void spi_disable_slave(spi_device_t *self)
{
    GPIO_SetState(self->cs, GPIO_High);
}

void spi_read_bytes(spi_device_t *self, uint8_t *buffer, int len)
{
    spi_hal_configure(self->clk, self->mode);
    spi_enable_slave(self);

    while(len-- > 0)
    {
        spi_hal_write_byte(0);
        while( !spi_hal_ready() ); // Wait end of transmission
        uint8_t byte = spi_hal_read_byte();
        WRITE_PU8(buffer++, byte);
    }

    spi_disable_slave(self);
}

void spi_read_byte(spi_device_t *self, uint8_t *byte)
{
    spi_hal_configure(self->clk, self->mode);
    spi_enable_slave(self);

    spi_hal_write_byte(0);
    while( !spi_hal_ready() ); // Wait end of transmission
    *byte = spi_hal_read_byte();

    spi_disable_slave(self);
}

void spi_write_bytes(spi_device_t *self, uint8_t *buffer, int len)
{
    spi_hal_configure(self->clk, self->mode);
    spi_enable_slave(self);

    while(len-- > 0)
    {
        uint8_t byte = READ_PU8(buffer);
        spi_hal_write_byte(byte);
        while( !spi_hal_ready() ); // Wait end of transmission
        byte = spi_hal_read_byte();
        WRITE_PU8(buffer++, byte);
    }

    spi_disable_slave(self);
}

void spi_write_byte(spi_device_t *self, uint8_t byte, uint8_t *read)
{
    spi_hal_configure(self->clk, self->mode);
    spi_enable_slave(self);

    spi_hal_write_byte(byte);
    while( !spi_hal_ready() ); // Wait end of transmission
    if (read != NULL_PTR) *read = spi_hal_read_byte();

    spi_disable_slave(self);
}
