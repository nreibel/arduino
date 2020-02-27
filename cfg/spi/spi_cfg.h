#ifndef SRC_SPI_CFG_SPI_CFG_H_
#define SRC_SPI_CFG_SPI_CFG_H_

#include "spi.h"

#define SPI_CLOCK_SELECT SPI_CLOCK_DIV_4
#define SPI_CLOCK_DIV_2X TRUE

typedef enum {
    SPI_Slave_TFT,
    SPI_NUMBER_OF_SLAVES
} SpiSlave;

#endif // SRC_SPI_CFG_SPI_CFG_H_
