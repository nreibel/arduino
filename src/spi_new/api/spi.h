#ifndef SRC_SPI_API_SPI_H_
#define SRC_SPI_API_SPI_H_

#include "types.h"
#include "gpio.h"

typedef enum {
    SPI_CLOCK_DIV_2,
    SPI_CLOCK_DIV_4,
    SPI_CLOCK_DIV_8,
    SPI_CLOCK_DIV_16,
    SPI_CLOCK_DIV_32,
    SPI_CLOCK_DIV_64,
    SPI_CLOCK_DIV_128
} spi_clock_e;

typedef enum {
    SPI_MODE_0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3
} spi_mode_e;


typedef struct {
    GPIO cs;
    spi_clock_e clk;
    spi_mode_e mode;
} spi_device_t;

void spi_init();
void spi_device_init(spi_device_t *self, GPIO cs, spi_clock_e clk, spi_mode_e mode);
void spi_disable_slave(spi_device_t *self);
void spi_enable_slave(spi_device_t *self);
void spi_read_byte(spi_device_t *self, uint8_t *byte);
void spi_read_bytes(spi_device_t *self, uint8_t *buffer, int len);

#endif // SRC_SPI_API_SPI_H_
