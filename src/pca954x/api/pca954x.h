#ifndef __PCA954X_API_H__
#define __PCA954X_API_H__

#include "i2c.h"
#include "types.h"
#include "os_cfg.h"

/*
 * Public constants
 */

#define PCA954X_MAX_NUMBER_OF_CHANNELS 8

/*
 * Exported types
 */

struct pca954x_prv_s;

typedef struct pca954x_bus_prv_s {
    struct i2c_bus_prv_s super;
    struct pca954x_prv_s * dev;
    int channel_id;
} * pca954x_bus_t;

typedef struct pca954x_prv_s {
    struct i2c_device_prv_s super;
    struct pca954x_bus_prv_s bus[PCA954X_MAX_NUMBER_OF_CHANNELS];
    struct pca954x_bus_prv_s * current;
    unsigned int nbr_of_channels;
} * pca954x_t;

/*
 * Exported functions
 */

#if OS_MALLOC
pca954x_t pca954x_create(i2c_bus_t parent, uint8_t addr, unsigned int nbr_of_channels);
pca954x_t pca9544_create(i2c_bus_t parent, uint8_t addr);
void pca954x_destroy(pca954x_t self);
#endif // OS_MALLOC

int pca954x_init(pca954x_t self, i2c_bus_t parent, uint8_t addr, unsigned int nbr_of_channels);
int pca9544_init(pca954x_t self, i2c_bus_t parent, uint8_t addr);

i2c_bus_t pca954x_get_bus(pca954x_t self, unsigned int bus);

#endif // __PCA954X_API_H__
