#ifndef __MAX31865_H__
#define __MAX31865_H__

#include "spi.h"

#define MAX_31865_STATUS_VOLTAGE     BIT(2) // Overvoltage/undervoltage fault
#define MAX_31865_STATUS_RTDIN_OPEN  BIT(3) // RTDIN < 0.85xVBIAS (open)
#define MAX_31865_STATUS_REFIN_OPEN  BIT(4) // REFIN < 0.85xVBIAS (open)
#define MAX_31865_STATUS_REFIN       BIT(5) // REFIN > 0.85xVBIAS
#define MAX_31865_STATUS_RTD_LOW     BIT(6) // RTD Low Threshold
#define MAX_31865_STATUS_RTD_HIGH    BIT(7) // RTD High Threshold

typedef struct {
    spi_device_t dev;
} max31865_t;

typedef enum {
    MAX31865_MODE_2_WIRES,
    MAX31865_MODE_3_WIRES,
    MAX31865_MODE_4_WIRES,
} max31865_mode_t;

typedef enum {
    MAX31865_FILTER_50HZ,
    MAX31865_FILTER_60HZ,
} max31865_filter_t;

int max31865_init(max31865_t *self, gpio_t *cs, max31865_mode_t mode, max31865_filter_t filter);
int max31865_read_rtd(max31865_t *self, double *rtd, bool *fault);
int max31865_read_fault_status(max31865_t *self, uint8_t *status);

double max31865_rtd_to_temperature(double rtd);

#endif // __MAX31865_H__
