#ifndef __MAX31790_API_H__
#define __MAX31790_API_H__

#include "types.h"
#include "i2c.h"
#include "os_cfg.h"

typedef enum {
    MAX31790_OK,
    MAX31790_ERROR,
    MAX31790_INVALID_MODE,
    MAX31790_INVALID_ARGUMENT,
    MAX31790_FAN_FAULT,
    MAX31790_HW_ERROR
} max31790_error_t;

typedef enum {
    MAX31790_FAN_1,
    MAX31790_FAN_2,
    MAX31790_FAN_3,
    MAX31790_FAN_4,
    MAX31790_FAN_5,
    MAX31790_FAN_6,
    MAX31790_NUMBER_OF_FANS
} max31790_fan_t;

typedef enum {
    MAX31790_TACH_1,
    MAX31790_TACH_2,
    MAX31790_TACH_3,
    MAX31790_TACH_4,
    MAX31790_TACH_5,
    MAX31790_TACH_6,
    MAX31790_TACH_7,
    MAX31790_TACH_8,
    MAX31790_TACH_9,
    MAX31790_TACH_10,
    MAX31790_TACH_11,
    MAX31790_TACH_12,
    MAX31790_NUMBER_OF_TACHS
} max31790_tach_t;

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
    MAX31790_MODE_RPM,
    MAX31790_MODE_TACHY
} max31790_mode_t;

typedef struct max31790_prv_s {
    struct i2c_device_prv_s dev;
    max31790_mode_t mode[MAX31790_NUMBER_OF_FANS];
} * max31790_t;

#if OS_MALLOC
max31790_t max31790_create(i2c_bus_t bus, uint8_t addr);
void max31790_destroy(max31790_t self);
#endif // OS_MALLOC

int max31790_init(max31790_t self, i2c_bus_t bus, uint8_t addr);
int max31790_set_target_rpm(max31790_t self, max31790_fan_t fan, unsigned int rpm);
int max31790_set_target_pwm(max31790_t self, max31790_fan_t fan, uint8_t pwm);
int max31790_get_fan_mode(max31790_t self, max31790_fan_t fan, max31790_mode_t *mode);
int max31790_set_fan_mode(max31790_t self, max31790_fan_t fan, max31790_mode_t mode);
int max31790_set_frequency(max31790_t self, max31790_frequency_t freq);
int max31790_set_watchdog(max31790_t self, max31790_watchdog_t watchdog);
int max31790_clear_watchdog(max31790_t self);
int max31790_read_rpm(max31790_t self, max31790_tach_t tach, unsigned int *rpm);

#endif // __MAX31790_API_H__
