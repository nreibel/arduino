#ifndef SRC_SPI_API_SPI_PRV_H_
#define SRC_SPI_API_SPI_PRV_H_

#include "port.h"
#include "spi_cfg.h"

#ifndef SPI_CLOCK_SELECT
#error SPI_CLOCK_SELECT is not set
#elif (SPI_CLOCK_SELECT == SPI_CLOCK_DIV_4)
#define SPI_CLOCK_SELECT_BITS 0
#elif (SPI_CLOCK_SELECT == SPI_CLOCK_DIV_16)
#define SPI_CLOCK_SELECT_BITS BIT(SPR0)
#elif (SPI_CLOCK_SELECT == SPI_CLOCK_DIV_64)
#define SPI_CLOCK_SELECT_BITS BIT(SPR1)
#elif (SPI_CLOCK_SELECT == SPI_CLOCK_DIV_128)
#define SPI_CLOCK_SELECT_BITS BIT(SPR0) | BIT(SPR1)
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

void Spi_Init_HW(void);

#endif // SRC_SPI_API_SPI_PRV_H_
