#ifndef I2C_DRV_API_H__
#define I2C_DRV_API_H__

#include "i2c.h"
#include "i2c_ll.h"
#include "types.h"

enum {
    I2C_DRV_OK,
    I2C_DRV_FAIL,
    I2C_DRV_INSTANCE
};

typedef struct i2c_atmega_bus_prv_s {
    struct i2c_bus_prv_s super;
    twi_t instance;
    bool fast_mode;
} * i2c_atmega_bus_t;

#if OS_MALLOC
i2c_bus_t i2c_driver_create(twi_t twi, bool fast_mode);
void i2c_driver_destroy(i2c_bus_t self);
#endif

int i2c_driver_init(i2c_atmega_bus_t bus, twi_t twi, bool fast_mode);

#endif // I2C_DRV_API_H__
