#include "max31790.h"
#include "types.h"
#include "bits.h"
#include "i2c.h"
#include "os.h"

/*
 * Private constants
 */

#define MAX31790_PULSE_PER_REV 2 // NP
#define MAX31790_TACH_PERIODS  4 // SR

#define MAX31790_REG_GLOBAL_CONFIGURATION 0x00
#define MAX31790_REG_PWM_FREQUENCY        0x01
#define MAX31790_REG_FAN_FAULT_STATUS_2   0x10
#define MAX31790_REG_FAN_FAULT_STATUS_1   0x11
#define MAX31790_REG_FAN_FAULT_MASK_2     0x12
#define MAX31790_REG_FAN_FAULT_MASK_1     0x13

/*
 * Private functions prototypes
 */

static int max31790_read_tachy(max31790_t self, max31790_tach_t fan, unsigned int *tach);
static int max31790_set_target_pwm(max31790_t self, max31790_fan_t fan, uint16_t pwm);
static int max31790_set_target_rpm(max31790_t self, max31790_fan_t fan, uint16_t rpm);

/*
 * Public functions
 */


#if OS_MALLOC

max31790_t max31790_create(i2c_bus_t bus, uint8_t addr)
{
    max31790_t instance = os_malloc(sizeof(*instance));
    if (instance != NULL_PTR) max31790_init(instance, bus, addr);
    return instance;
}

void max31790_destroy(max31790_t self)
{
    os_free(self);
}

#endif // OS_MALLOC

int max31790_init(max31790_t self, i2c_bus_t bus, uint8_t addr)
{
    int ret = i2c_device_init(&self->dev, bus, addr);
    if (ret < 0) return ret;

    // Bus timeout disabled
    uint8_t cfg = BIT(5);

    for (int i = 0 ; i < MAX31790_NUMBER_OF_FANS ; i++)
        self->mode[i] = MAX31790_MODE_PWM;

    // Write new config
    ret = i2c_device_write_byte(&self->dev, MAX31790_REG_GLOBAL_CONFIGURATION, cfg);
    if (ret < 0) return ret;

    return MAX31790_OK;
}

int max31790_set_target_speed(max31790_t self, max31790_fan_t fan, uint16_t target)
{
    // TODO : check fan

    switch(self->mode[fan])
    {
        case MAX31790_MODE_PWM:   return max31790_set_target_pwm(self, fan, target);
        case MAX31790_MODE_RPM:   return max31790_set_target_rpm(self, fan, target);
        case MAX31790_MODE_TACHY: return -MAX31790_INVALID_MODE;
        default: return -MAX31790_ERROR;
    }
}

int max31790_set_frequency(max31790_t self, max31790_frequency_t freq)
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
        default: return -MAX31790_INVALID_ARGUMENT;
    }

    int ret = i2c_device_write_byte(&self->dev, MAX31790_REG_PWM_FREQUENCY, regval);
    if (ret < 0) return ret;

    return MAX31790_OK;
}

int max31790_set_watchdog(max31790_t self, max31790_watchdog_t watchdog)
{
    uint8_t reg = 0;

    int ret = i2c_device_read_byte(&self->dev, MAX31790_REG_GLOBAL_CONFIGURATION, &reg);
    if (ret < 0) return ret;

    switch(watchdog)
    {
        case MAX31790_WATCHDOG_DISABLED:   SET_BITS(reg, 0x00, 0x6); break;
        case MAX31790_WATCHDOG_5_SECONDS:  SET_BITS(reg, 0x02, 0x6); break;
        case MAX31790_WATCHDOG_10_SECONDS: SET_BITS(reg, 0x04, 0x6); break;
        case MAX31790_WATCHDOG_30_SECONDS: SET_BITS(reg, 0x06, 0x6); break;
        default: return -MAX31790_INVALID_ARGUMENT;
    }

    ret = i2c_device_write_byte(&self->dev, MAX31790_REG_GLOBAL_CONFIGURATION, reg);
    if (ret < 0) return ret;

    return MAX31790_OK;
}

int max31790_clear_watchdog(max31790_t self)
{
    uint8_t reg = 0;

    int ret = i2c_device_read_byte(&self->dev, MAX31790_REG_GLOBAL_CONFIGURATION, &reg);
    if (ret < 0) return ret;

    RESET_BIT(reg, 0);

    ret = i2c_device_write_byte(&self->dev, MAX31790_REG_GLOBAL_CONFIGURATION, reg);
    if (ret < 0) return ret;

    return MAX31790_OK;
}

max31790_mode_t max31790_get_fan_mode(max31790_t self, max31790_fan_t fan)
{
    // TODO : check fan
    return self->mode[fan];
}

int max31790_set_fan_mode(max31790_t self, max31790_fan_t fan, max31790_mode_t mode)
{
    uint8_t reg = 0;
    uint8_t val = 0;

    switch(fan)
    {
        case MAX31790_FAN_1: reg = 0x02; break;
        case MAX31790_FAN_2: reg = 0x03; break;
        case MAX31790_FAN_3: reg = 0x04; break;
        case MAX31790_FAN_4: reg = 0x05; break;
        case MAX31790_FAN_5: reg = 0x06; break;
        case MAX31790_FAN_6: reg = 0x07; break;
        default: return -MAX31790_INVALID_ARGUMENT;
    }

    int ret = i2c_device_read_byte(&self->dev, reg, &val);
    if (ret < 0) return ret;

    switch(mode)
    {
        case MAX31790_MODE_PWM: RESET_BIT(val, 7); break;
        case MAX31790_MODE_RPM:   SET_BIT(val, 7); break;
        case MAX31790_MODE_TACHY: SET_BIT(val, 0); break;
        default: return -MAX31790_INVALID_ARGUMENT;
    }

    self->mode[fan] = mode;

    ret = i2c_device_write_byte(&self->dev, reg, val);
    if (ret < 0) return ret;

    return MAX31790_OK;
}

int max31790_read_rpm(max31790_t self, max31790_tach_t tachy, unsigned int *rpm)
{
    unsigned int val = 0;

    int ret = max31790_read_tachy(self, tachy, &val);
    if (ret < 0) return ret;

    *rpm = (60 * MAX31790_TACH_PERIODS * 8192ULL) / (MAX31790_PULSE_PER_REV * val);

    return MAX31790_OK;
}

/*
 * Private functions
 */

static int max31790_set_target_rpm(max31790_t self, max31790_fan_t fan, uint16_t rpm)
{
    uint8_t reg = 0;

    switch(fan)
    {
        case MAX31790_FAN_1: reg = 0x50; break;
        case MAX31790_FAN_2: reg = 0x52; break;
        case MAX31790_FAN_3: reg = 0x54; break;
        case MAX31790_FAN_4: reg = 0x56; break;
        case MAX31790_FAN_5: reg = 0x58; break;
        case MAX31790_FAN_6: reg = 0x5A; break;
        default: return -MAX31790_INVALID_ARGUMENT;
    }

    uint8_t bytes[2] = {
        rpm >> 3,
        rpm << 5
    };

    int ret = i2c_device_write_bytes(&self->dev, reg, bytes, 2);
    if (ret < 0) return ret;

    return MAX31790_OK;
}

static int max31790_set_target_pwm(max31790_t self, max31790_fan_t fan, uint16_t pwm)
{
    uint8_t reg = 0;

    switch(fan)
    {
        case MAX31790_FAN_1: reg = 0x40; break;
        case MAX31790_FAN_2: reg = 0x42; break;
        case MAX31790_FAN_3: reg = 0x44; break;
        case MAX31790_FAN_4: reg = 0x46; break;
        case MAX31790_FAN_5: reg = 0x48; break;
        case MAX31790_FAN_6: reg = 0x4A; break;
        default: return -MAX31790_INVALID_ARGUMENT;
    }

    uint8_t bytes[2] = {
        pwm >> 1,
        pwm << 7
    };

    int ret = i2c_device_write_bytes(&self->dev, reg, bytes, 2);
    if (ret < 0) return ret;

    return MAX31790_OK;
}

static int max31790_read_tachy(max31790_t self, max31790_tach_t tachy, unsigned int *value)
{
    uint8_t reg = 0;

    switch(tachy)
    {
        case MAX31790_TACH_1:  reg = 0x18; break;
        case MAX31790_TACH_2:  reg = 0x1A; break;
        case MAX31790_TACH_3:  reg = 0x1C; break;
        case MAX31790_TACH_4:  reg = 0x1E; break;
        case MAX31790_TACH_5:  reg = 0x20; break;
        case MAX31790_TACH_6:  reg = 0x22; break;
        case MAX31790_TACH_7:  reg = 0x24; break;
        case MAX31790_TACH_8:  reg = 0x26; break;
        case MAX31790_TACH_9:  reg = 0x28; break;
        case MAX31790_TACH_10: reg = 0x2A; break;
        case MAX31790_TACH_11: reg = 0x2C; break;
        case MAX31790_TACH_12: reg = 0x2E; break;
        default: return -MAX31790_INVALID_ARGUMENT;
    }

    // TODO : check fan mode

    word read = {0};
    i2c_device_read_bytes(&self->dev, reg, &read, 2);

    if ( MASK(read.bytes[1], 0x1F) != 0 )
    {
        // Read wrong value, HW error?
        return -MAX31790_HW_ERROR;
    }

    if ( read.value== 0xE0FF )
    {
        // Fan fault or stopped
        return -MAX31790_FAN_FAULT;
    }

    // Swap bytes and shift result
    word *w = TYPECAST(value, word*);
    w->bytes[0] = read.bytes[1];
    w->bytes[1] = read.bytes[0];
    w->value >>= 5;

    return MAX31790_OK;
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
