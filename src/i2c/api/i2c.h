#ifndef __I2C_API_I2C_H__
#define __I2C_API_I2C_H__

#include "types.h"
#include "i2c_cfg.h"

typedef enum {
    I2C_Mode_Master,
    I2C_Mode_Slave
} I2C_Mode;

Std_ReturnType I2C_Init(I2C_Mode i2cMode, byte address);
Std_ReturnType I2C_SetSlaveBuffer(void* buffer, int bufferLen);
Std_ReturnType I2C_ReadSync(uint8_t addr, void *buffer, int writeLen, int readLen, int delay);
Std_ReturnType I2C_WriteSync(uint8_t addr, void *buffer, int len);

// TODO : move to HAL
bool I2C_TransmitReady();
void I2C_StartCondition();
void I2C_RestartCondition();
void I2C_StopCondition();
void I2C_SlaveRead(uint8_t slaveAddr);
void I2C_SlaveWrite(uint8_t slaveAddr);
uint8_t I2C_Write(uint8_t data);
uint8_t I2C_ReadAck(void);
uint8_t I2C_ReadNak(void);

#if I2C_BACKGROUND_TASK == ON
Std_ReturnType I2C_BackgroundTask(void);
bool I2C_IsReady();
void I2C_SetupTransmission(uint8_t addr, void *buffer);
void I2C_StartWrite(int writeLen);
void I2C_StartRead(int writeLen, int readLen);
#endif // I2C_BACKGROUND_TASK

#endif // __I2C_API_I2C_H__
