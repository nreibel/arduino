#ifndef __MAX7219_PRV_H__
#define __MAX7219_PRV_H__

#include "types.h"
#include "spi.h"

#define MAX7219_MODE_NOOP       0x00
#define MAX7219_MODE_DECODE     0x09
#define MAX7219_MODE_INTENSITY  0x0A
#define MAX7219_MODE_SCAN_LIMIT 0x0B
#define MAX7219_MODE_POWER      0x0C
#define MAX7219_MODE_TEST       0x0F

void MAX7219_WriteRegister(Spi_Slave slave, uint8_t reg, uint8_t data);

#endif // __MAX7219_API_H__