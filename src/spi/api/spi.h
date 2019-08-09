#ifndef SRC_SPI_API_SPI_H_
#define SRC_SPI_API_SPI_H_

#include "types.h"
#include "spi_cfg.h"

#define SPI_SCK_FREQ_DIV4   0
#define SPI_SCK_FREQ_DIV16  1
#define SPI_SCK_FREQ_DIV64  2
#define SPI_SCK_FREQ_DIV128 3

Std_ReturnType Spi_BackgroundTask();

boolean Spi_IsReady();
void Spi_Init();
void Spi_SelectSlave(const Spi_Slave slave);
void Spi_DisableSlave(const Spi_Slave slave);
void Spi_FastWrite(const uint8_t write);
Std_ReturnType Spi_WriteByte(const uint8_t write, uint8_t *const read);
Std_ReturnType Spi_WriteBufferSync(const uint8_t *const data, const int length);
Std_ReturnType Spi_WriteBufferAsync(const uint8_t *const data, const int length);

#endif // SRC_SPI_API_SPI_H_
