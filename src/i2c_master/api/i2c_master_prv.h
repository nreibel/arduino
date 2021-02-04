#ifndef __I2C_API_I2C_PRV_H__
#define __I2C_API_I2C_PRV_H__

#define I2C_ADDR(addr, rw) (((addr) << 1) | rw)

bool I2C_Master_TransmitReady();
Std_ReturnType I2C_Master_StartCondition();
Std_ReturnType I2C_Master_RestartCondition();
Std_ReturnType I2C_Master_StopCondition();
Std_ReturnType I2C_Master_SlaveRead(uint8_t slaveAddr);
Std_ReturnType I2C_Master_SlaveWrite(uint8_t slaveAddr);
Std_ReturnType I2C_Master_Write(uint8_t data, uint8_t *status);
Std_ReturnType I2C_Master_ReadAck(uint8_t *data);
Std_ReturnType I2C_Master_ReadNak(uint8_t *data);

#endif // __I2C_API_I2C_PRV_H__
