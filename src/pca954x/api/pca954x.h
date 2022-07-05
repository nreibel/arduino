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

typedef struct pca954x_prv_s {
    struct i2c_device_prv_s super;
    uint8_t current;
} * pca954x_t;

/*
 * Exported functions
 */

#if OS_MALLOC
pca954x_t pca954x_create(i2c_bus_t parent, uint8_t addr);
void pca954x_destroy(pca954x_t self);
#endif // OS_MALLOC

int pca954x_init(pca954x_t self, i2c_bus_t parent, uint8_t addr);
int pca954x_select(pca954x_t self, uint8_t bus);

#endif // __PCA954X_API_H__
