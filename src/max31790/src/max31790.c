#include "max31790.h"
#include "max31790_prv.h"
#include "types.h"
#include "bits.h"
#include "i2c.h"

int max31790_init(max31790_t *self, i2c_bus_t bus, uint8_t addr)
{
    i2c_device_init(&self->dev, bus, addr);

    // Bus timeout disabled
    uint8_t cfg = BIT(5);

    // Write new config
    return i2c_device_write_byte(&self->dev, MAX31790_REG_GLOBAL_CONFIGURATION, cfg);
}

int max31790_set_target_rpm(max31790_t *self, max31790_fan_t fan, uint16_t rpm)
{
    uint8_t reg = 0;

    rpm = MASK(rpm, 0x1FF);

    switch(fan)
    {
        case MAX31790_FAN_0: reg = 0x50; break;
        case MAX31790_FAN_1: reg = 0x52; break;
        case MAX31790_FAN_2: reg = 0x54; break;
        case MAX31790_FAN_3: reg = 0x56; break;
        case MAX31790_FAN_4: reg = 0x58; break;
        case MAX31790_FAN_5: reg = 0x5A; break;
        default: return -1;
    }

    // TODO : check byte order
    return i2c_device_write_bytes(&self->dev, reg, &rpm, 2);
}

int max31790_set_target_pwm(max31790_t *self, max31790_fan_t fan, uint16_t pwm)
{
    uint8_t reg = 0;

    pwm = MASK(pwm, 0x1FF);

    switch(fan)
    {
        case MAX31790_FAN_0: reg = 0x40; break;
        case MAX31790_FAN_1: reg = 0x42; break;
        case MAX31790_FAN_2: reg = 0x44; break;
        case MAX31790_FAN_3: reg = 0x46; break;
        case MAX31790_FAN_4: reg = 0x48; break;
        case MAX31790_FAN_5: reg = 0x4A; break;
        default: return -1;
    }

    // TODO : check byte order
    return i2c_device_write_bytes(&self->dev, reg, &pwm, 2);
}

int max31790_set_frequency(max31790_t *self, max31790_frequency_t freq)
{
    uint8_t regval = 0;

    switch(freq)
    {
        case MAX31790_FREQUENCY_25_HZ:    regval = 0x00; break;
        case MAX31790_FREQUENCY_30_HZ:    regval = 0x11; break;
        case MAX31790_FREQUENCY_35_HZ:    regval = 0x22; break;
        case MAX31790_FREQUENCY_100_HZ:   regval = 0x33; break;
        case MAX31790_FREQUENCY_125_HZ:   regval = 0x44; break;
        case MAX31790_FREQUENCY_149_7_HZ: regval = 0x55; break;
        case MAX31790_FREQUENCY_1_25_KHZ: regval = 0x66; break;
        case MAX31790_FREQUENCY_1_47_KHZ: regval = 0x77; break;
        case MAX31790_FREQUENCY_3_57_KHZ: regval = 0x88; break;
        case MAX31790_FREQUENCY_5_KHZ:    regval = 0x99; break;
        case MAX31790_FREQUENCY_12_5_KHZ: regval = 0xAA; break;
        case MAX31790_FREQUENCY_25_KHZ:   regval = 0xBB; break;
        default: return -1;
    }

    return i2c_device_write_byte(&self->dev, MAX31790_REG_PWM_FREQUENCY, regval);
}

int max31790_set_watchdog(max31790_t *self, max31790_watchdog_t watchdog)
{
    uint8_t reg = 0;

    if ( i2c_device_read_byte(&self->dev, MAX31790_REG_GLOBAL_CONFIGURATION, &reg) < 0)
        return -1;

    switch(watchdog)
    {
        case MAX31790_WATCHDOG_DISABLED:   SET_BITS(reg, 0x00, 0x6); break;
        case MAX31790_WATCHDOG_5_SECONDS:  SET_BITS(reg, 0x02, 0x6); break;
        case MAX31790_WATCHDOG_10_SECONDS: SET_BITS(reg, 0x04, 0x6); break;
        case MAX31790_WATCHDOG_30_SECONDS: SET_BITS(reg, 0x06, 0x6); break;
        default: return -2;
    }

    return i2c_device_write_byte(&self->dev, MAX31790_REG_GLOBAL_CONFIGURATION, reg);
}

int max31790_clear_watchdog(max31790_t *self)
{
    uint8_t reg = 0;

    if ( i2c_device_read_byte(&self->dev, MAX31790_REG_GLOBAL_CONFIGURATION, &reg) < 0)
        return -1;

    RESET_BIT(reg, 0);

    return i2c_device_write_byte(&self->dev, MAX31790_REG_GLOBAL_CONFIGURATION, reg);
}

int max31790_set_fan_mode(max31790_t *self, max31790_fan_t fan, max31790_mode_t mode)
{
    uint8_t reg = 0;
    uint8_t val = 0;

    switch(fan)
    {
        case MAX31790_FAN_0: reg = 0x02; break;
        case MAX31790_FAN_1: reg = 0x03; break;
        case MAX31790_FAN_2: reg = 0x04; break;
        case MAX31790_FAN_3: reg = 0x05; break;
        case MAX31790_FAN_4: reg = 0x06; break;
        case MAX31790_FAN_5: reg = 0x07; break;
        default: return -1;
    }

    if ( i2c_device_read_byte(&self->dev, reg, &val) < 0 )
        return -2;

    switch(mode)
    {
        case MAX31790_MODE_PWM:
            RESET_BIT(val, 7);
            break;

        case MAX31790_MODE_RPM:
            SET_BIT(val, 7);
            break;

        default:
            return -3;
    }

    return i2c_device_write_byte(&self->dev, reg, val);
}

// Std_ReturnType MAX31790_SetStandby(bool stdby)
// {
//     uint8_t reg = 0;
//     i2c_device_read_byte(&dev, MAX31790_REG_GLOBAL_CONFIGURATION, &reg);
//     if (stdby) SET_BIT(reg, 7);
//     else RESET_BIT(reg, 7);
//     i2c_device_write_byte(&dev, MAX31790_REG_GLOBAL_CONFIGURATION, reg);
//     return Status_OK;
// }
//
// Std_ReturnType MAX31790_GetWatchdogStatus(bool *status)
// {
//     uint8_t reg = 0;
//     i2c_device_read_byte(&dev, MAX31790_REG_GLOBAL_CONFIGURATION, &reg);
//     *status = IS_SET_BIT(reg, 0);
//     return Status_OK;
// }
//
// Std_ReturnType MAX31790_SetFaultMask(uint8_t mask)
// {
//     mask = MASK(mask, 0x3F);
//     i2c_device_write_byte(&dev, MAX31790_REG_FAN_FAULT_MASK_1, mask);
//     return Status_OK;
// }
//
// Std_ReturnType MAX31790_GetFaultStatus(uint8_t *status)
// {
//     i2c_device_read_byte(&dev, MAX31790_REG_FAN_FAULT_STATUS_1, status);
//     return Status_OK;
// }
//
// Std_ReturnType MAX31790_GetRPM(MAX31790_Fan fan, uint16_t *rpm)
// {
//     uint16_t tach = 0;
//
//     if ( MAX31790_GetTachCount(fan, &tach) != Status_OK )
//     {
//         *rpm = 0;
//         return Status_Not_OK;
//     }
//     else
//     {
//         *rpm = (60 * MAX31790_TACH_PERIODS * 8192ULL) / (MAX31790_PULSE_PER_REV * tach);
//         return Status_OK;
//     }
// }
//
// Std_ReturnType MAX31790_GetTachCount(MAX31790_Fan fan, uint16_t *tach)
// {
//     uint8_t reg = 0;
//
//     switch(fan)
//     {
//         case MAX31790_FAN_1:
//             reg = 0x018;
//             break;
//
//         case MAX31790_FAN_2:
//             reg = 0x1A;
//             break;
//
//         case MAX31790_FAN_3:
//             reg = 0x1C;
//             break;
//
//         case MAX31790_FAN_4:
//             reg = 0x1E;
//             break;
//
//         case MAX31790_FAN_5:
//             reg = 0x20;
//             break;
//
//         case MAX31790_FAN_6:
//             reg = 0x22;
//             break;
//
//         default:
//             return Status_Not_OK;
//     }
//
//     word read = {0};
//     i2c_device_read_bytes(&dev, reg, &read, 2);
//
//     if ( MASK(read.bytes[1], 0x1F) != 0 )
//     {
//         // Read wrong value, HW error?
//         return Status_Not_OK;
//     }
//
//     if ( read.value== 0xe0ff )
//     {
//         // Fan fault or stopped
//         return Status_Not_OK;
//     }
//
//     // Swap bytes and shift result
//     word *w = TYPECAST(tach, word*);
//     w->bytes[0] = read.bytes[1];
//     w->bytes[1] = read.bytes[0];
//     w->value >>= 5;
//
//     return Status_OK;
// }
