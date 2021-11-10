#ifndef __PCA954X_API_H__
#define __PCA954X_API_H__

#include "types.h"
#include "i2c.h"

typedef struct pca954x_prv_s {
    struct i2c_device_prv_s dev;
    uint8_t current_channel;
} * pca954x_h;

int pca954x_init(pca954x_h self, i2c_bus_t bus, uint8_t addr);
int pca954x_disable(pca954x_h self);
int pca954x_select(pca954x_h self, uint8_t channel);

#endif /* __PCA954X_API_H__ */
