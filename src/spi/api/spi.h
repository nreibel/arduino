#ifndef SRC_SPI_API_SPI_H_
#define SRC_SPI_API_SPI_H_

#include "types.h"
#include "spi_cfg.h"

#define SPI_CLOCK_DIV_4   0
#define SPI_CLOCK_DIV_16  1
#define SPI_CLOCK_DIV_64  2
#define SPI_CLOCK_DIV_128 3

Std_ReturnType Spi_BackgroundTask();

boolean Spi_IsReady();
void Spi_Init();
void Spi_EnableSlave(uint8_t slave);
void Spi_DisableSlave(uint8_t slave);

Std_ReturnType Spi_WriteByte(uint8_t write, uint8_t *read);
Std_ReturnType Spi_WriteBufferSync(const void * const data, int length);
Std_ReturnType Spi_WriteBufferAsync(const void * const data, int length);

boolean Spi_HAL_IsReady(void);
void Spi_HAL_WriteByte(uint8_t write);
uint8_t Spi_HAL_ReadByte(void);

#endif // SRC_SPI_API_SPI_H_
