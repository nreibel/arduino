#ifndef SRC_SPI_API_SPI_PRV_H_
#define SRC_SPI_API_SPI_PRV_H_

#include "types.h"
#include "spi.h"
#include "gpio.h"

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
bool Spi_HAL_IsReady();
void Spi_HAL_WriteByte(uint8_t write);
uint8_t Spi_HAL_ReadByte();

#endif // SRC_SPI_API_SPI_PRV_H_
