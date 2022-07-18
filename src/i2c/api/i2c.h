#ifndef __I2C_API_H__
#define __I2C_API_H__

#include "i2c_ll.h"
#include "os_cfg.h"
#include "types.h"

enum {
    I2C_OK = 0,
    I2C_ERR,
    I2C_ERR_SEQ,
    I2C_ERR_PARAM,
    I2C_ERR_TIMEOUT,
};

typedef struct i2c_bus_prv_s {
    twi_t instance;
    bool fast_mode;
} * i2c_bus_t;

typedef struct i2c_device_prv_s {
    i2c_bus_t bus;
    uint8_t addr;
} * i2c_device_t;

int i2c_bus_init(i2c_bus_t self, twi_t twi, bool fast_mode);
// int i2c_bus_read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length);
// int i2c_bus_write(i2c_bus_t self, uint8_t addr, uint8_t reg, const void *data, unsigned int length);
int i2c_bus_transaction(i2c_bus_t self, uint8_t addr, const void * out, unsigned int wr, void * in, unsigned int rd, unsigned int timeout);

#if OS_MALLOC
i2c_device_t i2c_device_create(i2c_bus_t bus, uint8_t addr);
void i2c_device_destroy(i2c_device_t self);
#endif // OS_MALLOC

int i2c_device_init(i2c_device_t self, i2c_bus_t bus, uint8_t addr);
int i2c_device_transaction(i2c_device_t self, const void * out, unsigned int wr, void * in, unsigned int rd, unsigned int timeout);

#endif // __I2C_API_H__
