#ifndef SRC_SPI_API_SPI_H_
#define SRC_SPI_API_SPI_H_

#include "types.h"
#include "spi_cfg.h"

typedef enum {
    SPI_CLOCK_DIV_2,
    SPI_CLOCK_DIV_4,
    SPI_CLOCK_DIV_8,
    SPI_CLOCK_DIV_16,
    SPI_CLOCK_DIV_32,
    SPI_CLOCK_DIV_64,
    SPI_CLOCK_DIV_128
} Spi_Clock;

typedef enum {
    SPI_MODE_0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3
} Spi_Mode;

Std_ReturnType Spi_BackgroundTask();

bool Spi_IsReady();
void Spi_Init();
void Spi_Configure(Spi_Clock clock, Spi_Mode mode);
void Spi_EnableSlave(uint8_t slave);
void Spi_DisableSlave(uint8_t slave);

Std_ReturnType Spi_WriteByte(uint8_t write, uint8_t *read);
Std_ReturnType Spi_WriteBufferSync(const void * const data, int length);
Std_ReturnType Spi_WriteBufferAsync(const void * const data, int length);

bool Spi_HAL_IsReady(void);
void Spi_HAL_WriteByte(uint8_t write);
uint8_t Spi_HAL_ReadByte(void);

#endif // SRC_SPI_API_SPI_H_
