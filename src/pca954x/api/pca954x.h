#ifndef __PCA954X_API_H__
#define __PCA954X_API_H__

#include "types.h"

typedef struct {
    uint8_t i2c_addr;
} pca954x_t;

int pca954x_init(pca954x_t *self, uint8_t i2c_addr);
int pca954x_disable(pca954x_t *self);
int pca954x_select(pca954x_t *self, uint8_t channel);

#endif /* __PCA954X_API_H__ */
