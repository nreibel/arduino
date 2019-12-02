#ifndef SRC_SPI_API_SPI_PRV_H_
#define SRC_SPI_API_SPI_PRV_H_

#include "port.h"
#include "spi_cfg.h"

typedef enum {
	Spi_Uninitialized,
	Spi_Ready,
	Spi_Writing,
	Spi_Waiting,
} Spi_TransmitState;

extern PinDef Spi_MISO;
extern PinDef Spi_MOSI;
extern PinDef Spi_SCK;
extern PinDef SlaveSelect_Pins[];

void Spi_Init_HW(void);

#endif // SRC_SPI_API_SPI_PRV_H_
