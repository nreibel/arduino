#ifndef SRC_SPI_API_SPI_HAL_H_
#define SRC_SPI_API_SPI_HAL_H_

#include "types.h"
#include "spi.h"

void spi_hal_enable();
void spi_hal_configure(spi_clock_e clock, spi_mode_e mode);
bool spi_hal_ready();
void spi_hal_write_byte(uint8_t write);
uint8_t spi_hal_read_byte();

#endif // SRC_SPI_API_SPI_HAL_H_
