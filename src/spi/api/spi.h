#ifndef SPI_API_H__
#define SPI_API_H__

#include "types.h"
#include "spi_ll.h"
#include "gpio.h"
#include "os_cfg.h"

enum {
    SPI_OK,
    SPI_FAIL,
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
} * spi_device_t;

#if OS_MALLOC
spi_bus_t spi_bus_create(void);
void spi_bus_destroy(spi_bus_t dev);

spi_device_t spi_device_create(void);
void spi_device_destroy(spi_device_t dev);
#endif // OS_MALLOC

/*
 * SPI Bus
 */

int spi_bus_init(spi_bus_t self, spi_t dev);
int spi_bus_configure(spi_bus_t self, spi_clock_t clock, spi_mode_t mode);
int spi_bus_transfer(spi_bus_t self, const void * tx, void * rx, unsigned int len);

// Only works when SPI clock is set to SPI_CLOCK_DIV_2
void spi_bus_write_fast(spi_bus_t self, const uint8_t byte);

/*
 * SPI Device
 */

int spi_device_init(spi_device_t self, spi_bus_t bus, gpio_t cs, spi_clock_t clk, spi_mode_t mode);
int spi_device_enable(spi_device_t self);
int spi_device_disable(spi_device_t self);
int spi_device_transfer(spi_device_t self, const void * tx, void * rx, unsigned int len);

#endif // SPI_API_H__
