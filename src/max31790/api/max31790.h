#ifndef SRC_MAX31790_API_MAX31790_H_
#define SRC_MAX31790_API_MAX31790_H_

#include "types.h"

typedef enum {
    MAX31790_Watchdog_Disabled,
    MAX31790_Watchdog_5_Seconds,
    MAX31790_Watchdog_10_Seconds,
    MAX31790_Watchdog_30_Seconds
} MAX31790_Watchdog;

typedef enum {
    MAX31790_Frequency_25_Hz,
    MAX31790_Frequency_30_Hz,
    MAX31790_Frequency_35_Hz,
    MAX31790_Frequency_100_Hz,
    MAX31790_Frequency_125_Hz,
    MAX31790_Frequency_149_7_Hz,
    MAX31790_Frequency_1_25_kHz,
    MAX31790_Frequency_1_47_kHz,
    MAX31790_Frequency_3_57_kHz,
    MAX31790_Frequency_5_kHz,
    MAX31790_Frequency_12_5_kHz,
    MAX31790_Frequency_25_kHz,
} MAX31790_Frequency;

typedef enum {
    MAX31790_Fan1,
    MAX31790_Fan2,
    MAX31790_Fan3,
    MAX31790_Fan4,
    MAX31790_Fan5,
    MAX31790_Fan6,
    MAX31790_NUMBER_OF_FANS
} MAX31790_Fan;

typedef enum {
    MAX31790_FanMode_PWM,
    MAX31790_FanMode_RPM
} MAX31790_FanMode;

Std_ReturnType MAX31790_Init(MAX31790_Watchdog wd);
Std_ReturnType MAX31790_SetStandby(bool stdby);
Std_ReturnType MAX31790_ClearWatchdog();
Std_ReturnType MAX31790_SetFrequency(MAX31790_Frequency freq);
Std_ReturnType MAX31790_SetFanMode(MAX31790_Fan fan, MAX31790_FanMode mode);
Std_ReturnType MAX31790_SetTargetPWM(MAX31790_Fan fan, uint16_t pwm);
Std_ReturnType MAX31790_GetTachCount(MAX31790_Fan fan, uint16_t *tach);

#endif // SRC_MAX31790_API_MAX31790_H_
