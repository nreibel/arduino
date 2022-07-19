#ifndef __LTC4260_API_H__
#define __LTC4260_API_H__

#include "i2c.h"
#include "types.h"
#include "os_cfg.h"

/*
 * Exported types
 */

enum {
    LTC4260_OK,
    LTC4260_ERR_BUS,
};

typedef struct ltc4260_prv_s {
    struct i2c_device_prv_s super;
    uint8_t ctrl;
} * ltc4260_t;

/*
 * Exported functions
 */

#if OS_MALLOC
ltc4260_t ltc4260_create(i2c_bus_t parent, uint8_t addr, unsigned int timeout);
void ltc4260_destroy(ltc4260_t self);
#endif // OS_MALLOC

int ltc4260_init(ltc4260_t self, i2c_bus_t parent, uint8_t addr, unsigned int timeout);

int ltc4260_read_sense(ltc4260_t self, double * value, unsigned int timeout);
int ltc4260_read_source(ltc4260_t self, double * value, unsigned int timeout);
int ltc4260_read_adin(ltc4260_t self, double * value, unsigned int timeout);

int ltc4260_set_overcurrent_autoretry(ltc4260_t self, bool enabled, unsigned int timeout);
int ltc4260_set_overvoltage_autoretry(ltc4260_t self, bool enabled, unsigned int timeout);
int ltc4260_set_undervoltage_autoretry(ltc4260_t self, bool enabled, unsigned int timeout);

#endif // __LTC4260_API_H__
