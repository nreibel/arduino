#ifndef __I2C_API_H__
#define __I2C_API_H__

#include "types.h"

typedef enum {
    I2C_BUS_0,
    NUMBER_OF_I2C_BUSES
} i2c_bus_t;

typedef uint8_t (*tx_callback)(i2c_bus_t bus, unsigned int offset);
typedef void (*rx_callback)(i2c_bus_t bus, unsigned int offset, uint8_t data);

void i2c_bus_init_slave(i2c_bus_t bus, uint8_t addr);
void i2c_set_tx_callback(i2c_bus_t bus, tx_callback cbk);
void i2c_set_rx_callback(i2c_bus_t bus, rx_callback cbk);

#endif // __I2C_API_H__
