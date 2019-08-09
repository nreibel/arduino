#ifndef SRC_SPI_API_SPI_PRV_H_
#define SRC_SPI_API_SPI_PRV_H_

#include "port.h"
#include "spi_cfg.h"

#if defined SPI_SCK_FREQ && SPI_SCK_FREQ == SPI_SCK_FREQ_DIV4
#define SPI_PRV_SPCR_SPR 0x0
#elif defined SPI_SCK_FREQ && SPI_SCK_FREQ == SPI_SCK_FREQ_DIV16
#define SPI_PRV_SPCR_SPR 0x1
#elif defined SPI_SCK_FREQ && SPI_SCK_FREQ == SPI_SCK_FREQ_DIV64
#define SPI_PRV_SPCR_SPR 0x2
#elif defined SPI_SCK_FREQ && SPI_SCK_FREQ == SPI_SCK_FREQ_DIV128
#define SPI_PRV_SPCR_SPR 0x3
#else
#error SPI_SCK_FREQ is not correctly defined
#endif

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

#endif // SRC_SPI_API_SPI_PRV_H_
