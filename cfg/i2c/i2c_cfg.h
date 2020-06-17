#ifndef __I2C_CFG_I2C_CFG_H__
#define __I2C_CFG_I2C_CFG_H__

#define I2C_MODE_SLAVE  0
#define I2C_MODE_MASTER 1
#define I2C_MODE I2C_MODE_SLAVE

#if I2C_MODE == I2C_MODE_MASTER
    #define F_I2C 32000UL // In hertz
#elif I2C_MODE == I2C_MODE_SLAVE
    #define I2C_SLAVE_ADRESS 0x08
#else
    #error I2C_MODE not set
#endif

#endif // __I2C_CFG_I2C_CFG_H__
