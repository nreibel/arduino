#ifndef __TC74_API_H__
#define __TC74_API_H__

#include "types.h"
#include "i2c.h"

enum tc74_addr_e {
    TC74A0 = 0x48,
    TC74A1,
    TC74A2,
    TC74A3,
    TC74A4,
    TC74A5,
    TC74A6,
    TC74A7,
};

typedef struct tc74_prv_s {
    struct i2c_device_prv_s dev;
} * tc74_t;

void tc74_init(tc74_t self, i2c_bus_t bus, uint8_t addr);
int tc74_set_standby(tc74_t self, bool stdby);
int tc74_read_temperature(tc74_t self, int *temp);

#endif /* __TC74_API_H__ */
