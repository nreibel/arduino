#ifndef __I2C_API_I2C_PRV_H__
#define __I2C_API_I2C_PRV_H__

#define I2C_ADDR(addr, rw) (((addr) << 1) | rw)

void I2C_HAL_Init();
void I2C_HAL_EnablePeripheral();
bool I2C_HAL_TransmitReady();
void I2C_HAL_StartCondition();
void I2C_HAL_RestartCondition();
void I2C_HAL_StopCondition();
void I2C_HAL_SlaveRead(uint8_t slaveAddr);
void I2C_HAL_SlaveWrite(uint8_t slaveAddr);
uint8_t I2C_HAL_Write(uint8_t data);
uint8_t I2C_HAL_ReadAck(void);
uint8_t I2C_HAL_ReadNak(void);

#endif // __I2C_API_I2C_PRV_H__
