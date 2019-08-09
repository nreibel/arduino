#ifndef SRC_SPI_CFG_SPI_CFG_H_
#define SRC_SPI_CFG_SPI_CFG_H_

#include "spi.h"

#define SPI_SCK_FREQ SPI_SCK_FREQ_DIV4

typedef enum {
    Spi_Slave_TFT = 0,
    NbrOfSpiSlaves
} Spi_Slave;

#endif // SRC_SPI_CFG_SPI_CFG_H_
