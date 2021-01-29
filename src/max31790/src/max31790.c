#include "max31790.h"
#include "max31790_prv.h"
#include "types.h"
#include "bits.h"
#include "i2c_master.h"

Std_ReturnType MAX31790_Init(MAX31790_Watchdog wd)
{
    uint8_t reg = 0;

    SET_BIT(reg, 5); // Bus timeout disabled

    switch(wd)
    {
        case MAX31790_Watchdog_Disabled:
            // Nothing to do
            break;

        case MAX31790_Watchdog_5_Seconds:
            reg |= 0x2;
//             SET_MASK(reg, 0x2);
            break;

        case MAX31790_Watchdog_10_Seconds:
            reg |= 0x4;
//             SET_MASK(reg, 0x4);
            break;

        case MAX31790_Watchdog_30_Seconds:
            reg |= 0x6;
//             SET_MASK(reg, 0x6);
            break;
    }

    // Write new config
    return I2C_Master_WriteByteSync(0x20, MAX31790_REG_GLOBAL_CONFIGURATION, reg);
}

Std_ReturnType MAX31790_SetStandby(bool stdby)
{
    uint8_t reg = 0;
    I2C_Master_ReadByteSync(0x20, MAX31790_REG_GLOBAL_CONFIGURATION, &reg);
    if (stdby) SET_BIT(reg, 7);
    else RESET_BIT(reg, 7);
    I2C_Master_WriteByteSync(0x20, MAX31790_REG_GLOBAL_CONFIGURATION, reg);
    return Status_OK;
}

Std_ReturnType MAX31790_GetWatchdogStatus(bool *status)
{
    uint8_t reg = 0;
    I2C_Master_ReadByteSync(0x20, MAX31790_REG_GLOBAL_CONFIGURATION, &reg);
    *status = IS_SET_BIT(reg, 0);
    return Status_OK;
}

Std_ReturnType MAX31790_ClearWatchdog()
{
    uint8_t reg = 0;
    I2C_Master_ReadByteSync(0x20, MAX31790_REG_GLOBAL_CONFIGURATION, &reg);
    RESET_BIT(reg, 0);
    I2C_Master_WriteByteSync(0x20, MAX31790_REG_GLOBAL_CONFIGURATION, reg);
    return Status_OK;
}

Std_ReturnType MAX31790_SetFrequency(MAX31790_Frequency freq)
{
    uint8_t regval = 0;

    switch(freq)
    {
        case MAX31790_Frequency_25_Hz:
            regval = 0x00;
            break;

        case MAX31790_Frequency_30_Hz:
            regval = 0x11;
            break;

        case MAX31790_Frequency_35_Hz:
            regval = 0x22;
            break;

        case MAX31790_Frequency_100_Hz:
            regval = 0x33;
            break;

        case MAX31790_Frequency_125_Hz:
            regval = 0x44;
            break;

        case MAX31790_Frequency_149_7_Hz:
            regval = 0x55;
            break;

        case MAX31790_Frequency_1_25_kHz:
            regval = 0x66;
            break;

        case MAX31790_Frequency_1_47_kHz:
            regval = 0x77;
            break;

        case MAX31790_Frequency_3_57_kHz:
            regval = 0x88;
            break;

        case MAX31790_Frequency_5_kHz:
            regval = 0x99;
            break;

        case MAX31790_Frequency_12_5_kHz:
            regval = 0xAA;
            break;

        case MAX31790_Frequency_25_kHz:
            regval = 0xBB;
            break;

        default:
            return Status_Not_OK;
    }

    I2C_Master_WriteByteSync(0x20, MAX31790_REG_PWM_FREQUENCY, regval);

    return Status_OK;
}

Std_ReturnType MAX31790_SetFanMode(MAX31790_Fan fan, MAX31790_FanMode mode)
{
    uint8_t reg = 0;
    uint8_t val = 0;

    switch(fan)
    {
        case MAX31790_Fan1:
            reg = 0x02;
            break;

        case MAX31790_Fan2:
            reg = 0x03;
            break;

        case MAX31790_Fan3:
            reg = 0x04;
            break;

        case MAX31790_Fan4:
            reg = 0x05;
            break;

        case MAX31790_Fan5:
            reg = 0x06;
            break;

        case MAX31790_Fan6:
            reg = 0x07;
            break;

        default:
            return Status_Not_OK;
    }

    I2C_Master_ReadByteSync(0x20, reg, &val);

    switch(mode)
    {
        case MAX31790_FanMode_PWM:
            RESET_BIT(val, 7);
            break;

        case MAX31790_FanMode_RPM:
            SET_BIT(val, 7);
            break;

        default:
            return Status_Not_OK;
    }

    I2C_Master_WriteByteSync(0x20, reg, val);

    return Status_OK;
}

Std_ReturnType MAX31790_SetFaultMask(uint8_t mask)
{
    mask = MASK(mask, 0x3F);
    I2C_Master_WriteByteSync(0x20, MAX31790_REG_FAN_FAULT_MASK_1, mask);
    return Status_OK;
}

Std_ReturnType MAX31790_GetFaultStatus(uint8_t *status)
{
    I2C_Master_ReadByteSync(0x20, MAX31790_REG_FAN_FAULT_STATUS_1, status);
    return Status_OK;
}

Std_ReturnType MAX31790_SetTargetRPM(MAX31790_Fan fan, uint16_t rpm)
{
    uint8_t reg = 0;

    rpm = MASK(rpm, 0x1FF);

    switch(fan)
    {
        case MAX31790_Fan1:
            reg = 0x050;
            break;

        case MAX31790_Fan2:
            reg = 0x52;
            break;

        case MAX31790_Fan3:
            reg = 0x54;
            break;

        case MAX31790_Fan4:
            reg = 0x56;
            break;

        case MAX31790_Fan5:
            reg = 0x58;
            break;

        case MAX31790_Fan6:
            reg = 0x5A;
            break;

        default:
            return Status_Not_OK;
    }

    // TODO : check byte order
    I2C_Master_WriteSync(0x20, reg, &rpm, 2);

    return Status_OK;
}

Std_ReturnType MAX31790_SetTargetPWM(MAX31790_Fan fan, uint16_t pwm)
{
    uint8_t reg = 0;

    pwm = MASK(pwm, 0x1FF);

    switch(fan)
    {
        case MAX31790_Fan1:
            reg = 0x040;
            break;

        case MAX31790_Fan2:
            reg = 0x42;
            break;

        case MAX31790_Fan3:
            reg = 0x44;
            break;

        case MAX31790_Fan4:
            reg = 0x46;
            break;

        case MAX31790_Fan5:
            reg = 0x48;
            break;

        case MAX31790_Fan6:
            reg = 0x4A;
            break;

        default:
            return Status_Not_OK;
    }

    // TODO : check byte order
    I2C_Master_WriteSync(0x20, reg, &pwm, 2);

    return Status_OK;
}

Std_ReturnType MAX31790_GetRPM(MAX31790_Fan fan, uint16_t *rpm)
{
    uint16_t tach = 0;

    if ( MAX31790_GetTachCount(fan, &tach) != Status_OK )
    {
        *rpm = 0;
        return Status_Not_OK;
    }
    else
    {
        *rpm = (60 * MAX31790_TACH_PERIODS * 8192ULL) / (MAX31790_PULSE_PER_REV * tach);
        return Status_OK;
    }
}

Std_ReturnType MAX31790_GetTachCount(MAX31790_Fan fan, uint16_t *tach)
{
    uint8_t reg = 0;

    switch(fan)
    {
        case MAX31790_Fan1:
            reg = 0x018;
            break;

        case MAX31790_Fan2:
            reg = 0x1A;
            break;

        case MAX31790_Fan3:
            reg = 0x1C;
            break;

        case MAX31790_Fan4:
            reg = 0x1E;
            break;

        case MAX31790_Fan5:
            reg = 0x20;
            break;

        case MAX31790_Fan6:
            reg = 0x22;
            break;

        default:
            return Status_Not_OK;
    }

    word read = {0};
    I2C_Master_ReadSync(0x20, reg, &read, 2);

    if ( MASK(read.bytes[1], 0x1F) != 0 )
    {
        // Read wrong value, HW error?
        return Status_Not_OK;
    }

    if ( read.value== 0xe0ff )
    {
        // Fan fault or stopped
        return Status_Not_OK;
    }

    // Swap bytes and shift result
    word *w = TYPECAST(tach, word*);
    w->bytes[0] = read.bytes[1];
    w->bytes[1] = read.bytes[0];
    w->value >>= 5;

    return Status_OK;
}