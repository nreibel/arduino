#ifndef __PCA954X_API_H__
#define __PCA954X_API_H__

#include "i2c.h"
#include "types.h"
#include "os_cfg.h"

/*
 * Public constants
 */

#define PCA9544_NBR_OF_CHANNELS 4

/*
 * Exported types
 */

struct pca954x_prv_s;

typedef struct pca954x_bus_prv_s {
    struct i2c_bus_prv_s super;
    struct pca954x_prv_s * dev;
    uint8_t channel_id;
} * pca954x_bus_t;

typedef struct pca954x_prv_s {
    struct i2c_device_prv_s super;
    struct pca954x_bus_prv_s bus[PCA9544_NBR_OF_CHANNELS];
    struct pca954x_bus_prv_s * current;
} * pca9544_t;

/*
 * Exported functions
 */

#if OS_MALLOC
pca9544_t pca9544_create(i2c_bus_t parent, uint8_t addr);
void pca9544_destroy(pca9544_t self);
#endif // OS_MALLOC

int pca9544_init(pca9544_t self, i2c_bus_t parent, uint8_t addr);

i2c_bus_t pca9544_get_bus(pca9544_t self, unsigned int bus);

#endif // __PCA954X_API_H__
