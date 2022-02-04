#ifndef __SPI_API_LL_H
#define __SPI_API_LL_H

void spi_ll_enable();
void spi_ll_configure(spi_clock_t clock, spi_mode_t mode);
bool spi_ll_ready();
void spi_ll_write_byte(uint8_t write);
uint8_t spi_ll_read_byte();

#endif // __SPI_API_LL_H
