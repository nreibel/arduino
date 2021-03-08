#ifndef __I2C_PRV_H__
#define __I2C_PRV_H__

#include "types.h"

typedef struct {
    tx_callback tx_cbk;
    rx_callback rx_cbk;
} i2c_t;

#endif // __I2C_PRV_H__
