#ifndef SRC_SPI_API_SPI_PRV_H_
#define SRC_SPI_API_SPI_PRV_H_

#include "port.h"
#include "spi.h"
#include "spi_cfg.h"

typedef enum {
    Spi_Uninitialized,
    Spi_Ready,
    Spi_Writing,
    Spi_Waiting,
} Spi_TransmitState;

extern GPIO Spi_MISO;
extern GPIO Spi_MOSI;
extern GPIO Spi_SCK;
extern GPIO SlaveSelect_Pins[];

void Spi_HAL_Enable();
void Spi_HAL_Configure(Spi_Clock clock, Spi_Mode mode);

#endif // SRC_SPI_API_SPI_PRV_H_
