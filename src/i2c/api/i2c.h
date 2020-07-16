#ifndef __I2C_API_I2C_H__
#define __I2C_API_I2C_H__

#include "types.h"
#include "i2c_cfg.h"

Std_ReturnType I2C_Init();

#if I2C_MODE == I2C_MODE_MASTER
    Std_ReturnType I2C_ReadSync(uint8_t addr, void *buffer, int writeLen, int readLen, int delay);
    Std_ReturnType I2C_WriteSync(uint8_t addr, void *buffer, int len);
#elif I2C_MODE == I2C_MODE_SLAVE
    extern byte I2C_Transmit(int offset);
    extern void I2C_Receive(int offset, byte data);
#else
    #error I2C_MODE not set
#endif

#endif // __I2C_API_I2C_H__
