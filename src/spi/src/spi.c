#include "spi.h"
#include "gpio.h"
#include "bits.h"
#include <avr/io.h>

/*
 * Private functions declaration
 */

static void spi_ll_enable();
static void spi_ll_configure(spi_clock_t clock, spi_mode_t mode);
static bool spi_ll_ready();
static void spi_ll_write_byte(uint8_t write);
static uint8_t spi_ll_read_byte();

/*
 * Exported functions
 */

void spi_init()
{
    // Enable MISO, MOSI, SCK
    gpio_set_data_direction(MOSI, GPIO_OUTPUT_ACTIVE_HIGH);
    gpio_set_data_direction(MISO, GPIO_INPUT_HIGH_Z);
    gpio_set_data_direction(SCK, GPIO_OUTPUT_ACTIVE_HIGH);

    spi_ll_enable();
}

void spi_device_init(spi_device_t self, gpio_t cs, spi_clock_t clk, spi_mode_t mode)
{
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

void spi_write_byte(spi_device_t self, uint8_t byte, uint8_t *read)
{
    if (!self->transaction_mode) spi_enable_slave(self);

    spi_ll_write_byte(byte);
    while( !spi_ll_ready() ); // Wait end of transmission
    if (read != NULL_PTR) *read = spi_ll_read_byte();

    if (!self->transaction_mode) spi_disable_slave(self);
}

/*
 * Private functions
 */

static void spi_ll_enable()
{
    // Enable peripheral
    RESET_BIT(PRR, PRSPI);
}

static void spi_ll_configure(spi_clock_t clock, spi_mode_t mode)
{
    // Enable SPI, Master Mode
    uint8_t spcr = BIT(MSTR) | BIT(SPE);
    uint8_t spsr = 0;

    switch(clock)
    {
        case SPI_CLOCK_DIV_2:
            SET_BIT(spsr, SPI2X);
        case SPI_CLOCK_DIV_4:
            // Do nothing
            break;

        case SPI_CLOCK_DIV_8:
            SET_BIT(spsr, SPI2X);
        case SPI_CLOCK_DIV_16:
            SET_BIT(spcr, SPR0);
            break;

        case SPI_CLOCK_DIV_32:
            SET_BIT(spsr, SPI2X);
        case SPI_CLOCK_DIV_64:
            SET_BIT(spcr, SPR1);
            break;

        case SPI_CLOCK_DIV_128:
            SET_BIT(spcr, SPR0);
            SET_BIT(spcr, SPR1);
            break;
    }

    switch(mode)
    {
        case SPI_MODE_0:
            // Do nothing
            break;

        case SPI_MODE_1:
            SET_BIT(spcr, CPHA);
            break;

        case SPI_MODE_2:
            SET_BIT(spcr, CPOL);
            break;

        case SPI_MODE_3:
            SET_BIT(spcr, CPOL);
            SET_BIT(spcr, CPHA);
            break;
    }

    SPCR = spcr;
    SPSR = spsr;
}

static bool spi_ll_ready(void)
{
    return IS_SET_BIT(SPSR, SPIF);
}

static void spi_ll_write_byte(const uint8_t write)
{
    SPDR = write;
}

static uint8_t spi_ll_read_byte(void)
{
    return SPDR;
}
