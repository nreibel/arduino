#ifndef __PCA954X_API_H__
#define __PCA954X_API_H__

#include "i2c.h"
#include "types.h"

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

int pca954x_init(pca954x_t self, i2c_bus_t parent, uint8_t addr, unsigned int nbr_of_channels);
int pca9544_init(pca954x_t self, i2c_bus_t parent, uint8_t addr);

i2c_bus_t pca954x_get_bus(pca954x_t self, unsigned int bus);


#endif // __PCA954X_API_H__
