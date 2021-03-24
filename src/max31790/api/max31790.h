#ifndef __MAX31790_API_H__
#define __MAX31790_API_H__

#include "types.h"
#include "i2c.h"

typedef enum {
    MAX31790_FAN_0,
    MAX31790_FAN_1,
    MAX31790_FAN_2,
    MAX31790_FAN_3,
    MAX31790_FAN_4,
    MAX31790_FAN_5,
    MAX31790_NUMBER_OF_FANS
} max31790_fan_t;

typedef enum {
    MAX31790_FREQUENCY_25_HZ,
    MAX31790_FREQUENCY_30_HZ,
    MAX31790_FREQUENCY_35_HZ,
    MAX31790_FREQUENCY_100_HZ,
    MAX31790_FREQUENCY_125_HZ,
    MAX31790_FREQUENCY_149_7_HZ,
    MAX31790_FREQUENCY_1_25_KHZ,
    MAX31790_FREQUENCY_1_47_KHZ,
    MAX31790_FREQUENCY_3_57_KHZ,
    MAX31790_FREQUENCY_5_KHZ,
    MAX31790_FREQUENCY_12_5_KHZ,
    MAX31790_FREQUENCY_25_KHZ
} max31790_frequency_t;

typedef enum {
    MAX31790_WATCHDOG_DISABLED,
    MAX31790_WATCHDOG_5_SECONDS,
    MAX31790_WATCHDOG_10_SECONDS,
    MAX31790_WATCHDOG_30_SECONDS
} max31790_watchdog_t;

typedef enum {
    MAX31790_MODE_PWM,
    MAX31790_MODE_RPM
} max31790_mode_t;

typedef struct {
    i2c_device_t dev;
} max31790_t;

int max31790_init(max31790_t *self, i2c_bus_t bus, uint8_t addr);
int max31790_set_target_pwm(max31790_t *self, max31790_fan_t fan, uint16_t pwm);
int max31790_set_target_rpm(max31790_t *self, max31790_fan_t fan, uint16_t rpm);
int max31790_set_fan_mode(max31790_t *self, max31790_fan_t fan, max31790_mode_t mode);
int max31790_set_frequency(max31790_t *self, max31790_frequency_t freq);
int max31790_set_watchdog(max31790_t *self, max31790_watchdog_t watchdog);
int max31790_clear_watchdog(max31790_t *self);

#endif // __MAX31790_API_H__
