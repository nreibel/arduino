#ifndef __I2C_API_H__
#define __I2C_API_H__

#include "types.h"

typedef enum {
    I2C_BUS_0,
    NUMBER_OF_I2C_BUSES
} i2c_bus_t;

typedef struct i2c_device_prv_s {
    i2c_bus_t bus;
    uint8_t   addr;
} * i2c_device_t;

typedef uint8_t (*tx_callback)(i2c_bus_t bus, unsigned int offset);
typedef void (*rx_callback)(i2c_bus_t bus, unsigned int offset, uint8_t data);

int i2c_bus_init_slave(i2c_bus_t bus, uint8_t addr);
int i2c_bus_init_master(i2c_bus_t bus, bool fast_mode);

int i2c_device_init(i2c_device_t self, i2c_bus_t bus, uint8_t addr);

int i2c_device_write_byte(i2c_device_t self, uint8_t reg, uint8_t byte);
int i2c_device_write_bytes(i2c_device_t self, uint8_t reg, void *data, unsigned int length);

int i2c_device_read_byte(i2c_device_t self, uint8_t reg, uint8_t *data);
int i2c_device_read_bytes(i2c_device_t self, uint8_t reg, void *data, unsigned int length);

uint8_t i2c_device_get_addr(i2c_device_t self);

int i2c_set_tx_callback(i2c_bus_t bus, tx_callback cbk);
int i2c_set_rx_callback(i2c_bus_t bus, rx_callback cbk);

#endif // __I2C_API_H__
