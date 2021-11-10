#ifndef __I2C_LL_API_H__
#define __I2C_LL_API_H__

#include "types.h"

int i2c_ll_start_condition();
int i2c_ll_restart_condition();
int i2c_ll_stop_condition();
int i2c_ll_slave_write(uint8_t addr);
int i2c_ll_slave_read(uint8_t addr);
int i2c_ll_write(uint8_t data, uint8_t *status);
int i2c_ll_read_ack(uint8_t *data);
int i2c_ll_read_nack(uint8_t *data);

#endif // __I2C_LL_API_H__
