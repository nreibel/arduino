#ifndef __I2C_SLAVE_API_I2C_H__
#define __I2C_SLAVE_API_I2C_H__

#include "types.h"

Std_ReturnType I2C_Slave_Init(uint8_t addr);

extern byte I2C_Slave_TransmitCallback(int offset);
extern void I2C_Slave_ReceiveCallback(int offset, byte data);
extern void I2C_Slave_StartCallback();
extern void I2C_Slave_StopCallback();

#endif // __I2C_SLAVE_API_I2C_H__
