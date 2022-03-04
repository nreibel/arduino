#ifndef SRC_SPI_API_SPI_H_
#define SRC_SPI_API_SPI_H_

#include "types.h"
#include "gpio.h"
#include "os_cfg.h"

typedef enum {
    SPI_CLOCK_DIV_2,
    SPI_CLOCK_DIV_4,
    SPI_CLOCK_DIV_8,
    SPI_CLOCK_DIV_16,
    SPI_CLOCK_DIV_32,
    SPI_CLOCK_DIV_64,
    SPI_CLOCK_DIV_128,
    NUMBER_OF_SPI_CLOCK_DIV
} spi_clock_t;

typedef enum {
    SPI_MODE_0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3,
    NUMBER_OF_SPI_MODE
} spi_mode_t;

typedef enum {
    SPI_BUS_0,
    NUMBER_OF_SPI_BUS
} spi_bus_t;

typedef struct spi_device_prv_s{
    gpio_t cs;
    spi_clock_t clk;
    spi_mode_t mode;
    bool transaction_mode;
} * spi_device_t;

#ifdef OS_MALLOC
spi_device_t spi_device_create(spi_bus_t bus, gpio_t cs, spi_clock_t clk, spi_mode_t mode);
void spi_device_destroy(spi_device_t dev);
#endif // OS_MALLOC

void spi_init(spi_bus_t bus);
void spi_device_init(spi_device_t self, spi_bus_t bus, gpio_t cs, spi_clock_t clk, spi_mode_t mode);

void spi_set_transaction_mode_enabled(spi_device_t self, bool enabled);
void spi_disable_slave(spi_device_t self);
void spi_enable_slave(spi_device_t self);
void spi_read_byte(spi_device_t self, uint8_t *byte);
void spi_read_bytes(spi_device_t self, uint8_t *buffer, int len);
void spi_write_byte(spi_device_t self, uint8_t byte, uint8_t *read);
void spi_write_bytes(spi_device_t self, uint8_t *buffer, int len);
void spi_write_fast(spi_device_t self, const uint8_t *buffer, int len);

#endif // SRC_SPI_API_SPI_H_
