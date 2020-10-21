#ifndef __MAX7219_API_H__
#define __MAX7219_API_H__

#include "types.h"
#include "spi.h"

void MAX7219_Init(Spi_Slave slave);

void MAX7219_ClearDisplay(Spi_Slave slave);
void MAX7219_SetEnabled(Spi_Slave slave, bool enabled);
void MAX7219_SetBacklight(Spi_Slave slave, uint8_t value);
void MAX7219_SetDigit(Spi_Slave slave, int digit, uint8_t value);
int  MAX7219_InitStringBuffer(void *buffer, char* str, int len);

#endif // __MAX7219_API_H__
