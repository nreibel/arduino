#ifndef SPI_API_H__
#define SPI_API_H__

#include "types.h"
#include "spi_ll.h"
#include "gpio.h"
#include "os_cfg.h"

enum {
    SPI_OK,
    SPI_ERROR_PRESCALER,
    SPI_ERROR_POLARITY
};

typedef enum {
    SPI_CLOCK_DIV_2,
    SPI_CLOCK_DIV_4,
    SPI_CLOCK_DIV_8,
    SPI_CLOCK_DIV_16,
    SPI_CLOCK_DIV_32,
    SPI_CLOCK_DIV_64,
    SPI_CLOCK_DIV_128,
    NUMBER_OF_SPI_CLOCK_DIVS
} spi_clock_t;

typedef enum {
    SPI_MODE_0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3,
    NUMBER_OF_SPI_MODES
} spi_mode_t;

typedef struct spi_bus_prv_s {
    spi_t bus;
    spi_clock_t clock;
    spi_mode_t mode;
} * spi_bus_t;

typedef struct spi_device_prv_s {
    spi_bus_t bus;
    gpio_t cs;
    spi_clock_t clock;
    spi_mode_t mode;
    bool transaction_mode;
} * spi_device_t;

#if OS_MALLOC
spi_bus_t spi_bus_create(spi_t dev);
void spi_bus_destroy(spi_bus_t dev);

spi_device_t spi_device_create(spi_bus_t bus, gpio_t cs, spi_clock_t clk, spi_mode_t mode);
void spi_device_destroy(spi_device_t dev);
#endif // OS_MALLOC

/*
 * SPI Bus
 */

int spi_bus_init(spi_bus_t self, spi_t dev);
int spi_bus_configure(spi_bus_t self, spi_clock_t clock, spi_mode_t mode);
int spi_bus_read_byte(spi_bus_t self, uint8_t *byte);
int spi_bus_read_bytes(spi_bus_t self, void *buffer, int len);
int spi_bus_write_byte(spi_bus_t self, uint8_t byte, uint8_t *read);
int spi_bus_write_bytes(spi_bus_t self, void *buffer, int len);
int spi_bus_write_fast(spi_bus_t self, const uint8_t byte);

/*
 * SPI Device
 */

int spi_device_init(spi_device_t self, spi_bus_t bus, gpio_t cs, spi_clock_t clk, spi_mode_t mode);
int spi_device_set_transaction_mode(spi_device_t self, bool enabled);
int spi_device_disable(spi_device_t self);
int spi_device_enable(spi_device_t self);
int spi_device_read_byte(spi_device_t self, uint8_t *byte);
int spi_device_read_bytes(spi_device_t self, void *buffer, int len);
int spi_device_write_byte(spi_device_t self, uint8_t byte, uint8_t *read);
int spi_device_write_bytes(spi_device_t self, void *buffer, int len);
int spi_device_write_fast(spi_device_t self, const uint8_t byte);

#endif // SPI_API_H__
