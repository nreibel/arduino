#ifndef __SPI_API_LL_H
#define __SPI_API_LL_H

#include "types.h"

enum {
    SPI_LL_PSCL_4   = 0x0,
    SPI_LL_PSCL_16  = 0x1,
    SPI_LL_PSCL_64  = 0x2,
    SPI_LL_PSCL_128 = 0x3,
};

typedef union {
    struct {
        uint8_t spr  : 2;
        uint8_t cpha : 1;
        uint8_t cpol : 1;
        uint8_t mstr : 1;
        uint8_t dord : 1;
        uint8_t spe  : 1;
        uint8_t spie : 1;
    } bits;
    uint8_t reg;
} spcr_t;

typedef union {
    struct {
        uint8_t spi2x      : 1;
        uint8_t __reserved : 5;
        uint8_t wcol       : 1;
        uint8_t spif       : 1;
    } bits;
    uint8_t reg;
} spsr_t;

typedef struct {
    spcr_t spcr;
    spsr_t spsr;
    uint8_t spdr;
} volatile * spi_t;

#define SPI0 TYPECAST(0x4C, spi_t)

void spi_ll_init(spi_t spi);
void spi_ll_set_prescaler(spi_t spi, uint8_t prescaler);
void spi_ll_set_double_speed(spi_t spi, bool dbl);
void spi_ll_set_clock_polarity(spi_t spi, bool polarity);
void spi_ll_set_clock_phase(spi_t spi, bool phase);
void spi_ll_wait_tx(spi_t spi);
bool spi_ll_ready(spi_t spi);
void spi_ll_write_byte(spi_t spi, uint8_t write);
uint8_t spi_ll_read_byte(spi_t spi);

#endif // __SPI_API_LL_H
