#ifndef __I2C_API_H__
#define __I2C_API_H__

#include "i2c_ll.h"
#include "os_cfg.h"
#include "types.h"

enum {
    I2C_OK = 0,
    I2C_FAIL,
    I2C_SEQ_FAIL
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
int i2c_bus_read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length);
int i2c_bus_write(i2c_bus_t self, uint8_t addr, uint8_t reg, const void *data, unsigned int length);
int i2c_bus_transaction(i2c_bus_t self, uint8_t addr, void *data, unsigned int wr, unsigned int rd, unsigned int delay);

#if OS_MALLOC
i2c_device_t i2c_device_create(i2c_bus_t bus, uint8_t addr);
void i2c_device_destroy(i2c_device_t self);
#endif // OS_MALLOC

int i2c_device_init(i2c_device_t self, i2c_bus_t bus, uint8_t addr);
int i2c_device_write_byte(i2c_device_t self, uint8_t reg, const uint8_t byte);
int i2c_device_write_bytes(i2c_device_t self, uint8_t reg, const void *data, unsigned int length);
int i2c_device_read_byte(i2c_device_t self, uint8_t reg, uint8_t *data);
int i2c_device_read_bytes(i2c_device_t self, uint8_t reg, void *data, unsigned int length);
int i2c_device_transaction(i2c_device_t self, void *data, unsigned int wr, unsigned int rd, unsigned int delay);

#endif // __I2C_API_H__
