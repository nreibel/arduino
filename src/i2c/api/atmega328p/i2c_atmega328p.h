#ifndef __I2C_ATMEGA328P_API_H__
#define __I2C_ATMEGA328P_API_H__

#include "types.h"

#define NUMBER_OF_I2C_BUS 1

int i2c_atmega328p_init(bool fast_mode);

#endif // __I2C_ATMEGA328P_API_H__
