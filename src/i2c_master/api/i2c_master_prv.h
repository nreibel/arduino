#ifndef __I2C_API_I2C_PRV_H__
#define __I2C_API_I2C_PRV_H__

#define I2C_ADDR(addr, rw) (((addr) << 1) | rw)

bool I2C_Master_TransmitReady();
void I2C_Master_StartCondition();
void I2C_Master_RestartCondition();
void I2C_Master_StopCondition();
void I2C_Master_SlaveRead(uint8_t slaveAddr);
void I2C_Master_SlaveWrite(uint8_t slaveAddr);
uint8_t I2C_Master_Write(uint8_t data);
uint8_t I2C_Master_ReadAck(void);
uint8_t I2C_Master_ReadNak(void);

#endif // __I2C_API_I2C_PRV_H__
