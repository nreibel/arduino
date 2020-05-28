#ifndef __I2C_API_I2C_PRV_H__
#define __I2C_API_I2C_PRV_H__

#define I2C_ADDR(addr, rw) (((addr) << 1) | rw)


typedef enum {
    I2C_State_Uninitialized = 0,
    I2C_State_MasterReady,
    I2C_State_SlaveReady,

#if I2C_BACKGROUND_TASK == ON
    I2C_State_Start,
    I2C_State_RepStart,
    I2C_State_Slave_Write,
    I2C_State_Slave_Read,
    I2C_State_Stop,
    I2C_State_Read,
    I2C_State_Write,
#endif // if I2C_BACKGROUND_TASK

} I2C_State;

#endif // __I2C_API_I2C_PRV_H__
