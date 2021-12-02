#ifndef __I2C_ATMEGA328P_API_H__
#define __I2C_ATMEGA328P_API_H__

#include "i2c.h"
#include "types.h"

typedef struct i2c_atmega328p_bus_prv_s {
    struct i2c_bus_prv_s super;
    bool fast_mode; // 400kHz
} * i2c_atmega328p_bus_t;

extern const i2c_atmega328p_bus_t I2C_BUS_0;

int i2c_atmega328p_init(i2c_atmega328p_bus_t self, bool fast_mode);

#endif // __I2C_ATMEGA328P_API_H__
