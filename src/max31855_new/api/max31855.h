#ifndef SRC_MAX31855_API_MAX31855_H_
#define SRC_MAX31855_API_MAX31855_H_

#include "spi.h"

typedef struct {
    spi_device_t spi_device;
} max31855_t;

void max31855_device_init(max31855_t *self, GPIO cs);
void max31855_get_internal_temperature(max31855_t *self, double *temperature);
bool max31855_get_temperature(max31855_t *self, double *temperature);

#endif // SRC_MAX31855_API_MAX31855_H_
