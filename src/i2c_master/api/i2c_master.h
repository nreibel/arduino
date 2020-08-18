#ifndef __I2C_API_I2C_H__
#define __I2C_API_I2C_H__

#include "types.h"

Std_ReturnType I2C_Master_Init();

Std_ReturnType I2C_Master_ReadSync(uint8_t addr, buffer_t buffer, int writeLen, int readLen, int delay);
Std_ReturnType I2C_Master_WriteSync(uint8_t addr, buffer_t buffer, int len);

#endif // __I2C_API_I2C_H__
