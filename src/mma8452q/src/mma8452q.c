#include "i2c_master.h"
#include "mma8452q.h"
#include "mma8452q_cfg.h"
#include "mma8452q_prv.h"
#include "types.h"
#include "bits.h"

Std_ReturnType MMA8452Q_Init()
{
    uint8_t whoami = 0;
    uint8_t ctrl_reg1 = 0;

    // Read WHO_AM_I to ensure communication
    I2C_Master_ReadByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_WHO_AM_I, &whoami);
    if (whoami != MMA8452Q_DEVICE_ID) return Status_Not_OK;

#if MMA8452Q_READ_MODE_FAST == ON
    SET_BIT(ctrl_reg1, MMA8452Q_CTRL_REG1_F_READ);
#endif

    // Device must be in standby before writing config
    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG1, 0);
    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG1, ctrl_reg1);

    return Status_OK;
}

void MMA8452Q_Reset()
{
    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG2, BIT(6) );
}

void MMA8452Q_GetStatus(uint8_t* status)
{
    I2C_Master_ReadByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_STATUS, status);
}

Std_ReturnType MMA8452Q_SetRange(MMA8452Q_Range_t range, bool hpf)
{
    uint8_t xyz_data_cfg = 0;

    switch(range)
    {
        case MMA8452Q_Range_2G:
            // Nothing to do
            break;
        case MMA8452Q_Range_4G:
            SET_BIT(xyz_data_cfg, MMA8452Q_XYZ_DATA_CFG_FS0);
            break;
        case MMA8452Q_Range_8G:
            SET_BIT(xyz_data_cfg, MMA8452Q_XYZ_DATA_CFG_FS1);
            break;
        default:
            return Status_Invalid_Parameter;
    }

    if (hpf) SET_BIT(xyz_data_cfg, MMA8452Q_XYZ_DATA_CFG_HPF_OUT);

    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_XYZ_DATA_CFG, xyz_data_cfg);

    return Status_OK;
}

void MMA8452Q_SetStandby(bool standby)
{
    uint8_t ctrl_reg1 = 0;
    I2C_Master_ReadByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG1, &ctrl_reg1);

    if (standby) RESET_BIT(ctrl_reg1, MMA8452Q_CTRL_REG1_ACTIVE);
    else SET_BIT(ctrl_reg1, MMA8452Q_CTRL_REG1_ACTIVE);

    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG1, ctrl_reg1);
}

void MMA8452Q_GetData(MMA8452Q_Data_t* buffer)
{
    I2C_Master_ReadSync(MMA8452Q_I2C_ADDR, MMA8452Q_OUT_X_MSB, buffer, sizeof(MMA8452Q_Data_t));

#if MMA8452Q_READ_MODE_FAST != ON
    uint8_t *b = PU8(buffer);

    buffer->acc_x = (b[0] << 4) | (b[1] >> 4);
    buffer->acc_y = (b[2] << 4) | (b[3] >> 4);
    buffer->acc_z = (b[4] << 4) | (b[5] >> 4);

    if (buffer->acc_x > 0x7FF) buffer->acc_x -= 0xFFF;
    if (buffer->acc_y > 0x7FF) buffer->acc_y -= 0xFFF;
    if (buffer->acc_z > 0x7FF) buffer->acc_z -= 0xFFF;
#endif
}

void MMA8452Q_GetInterruptStatus(uint8_t* mask)
{
    uint8_t bit = 0;
    uint8_t int_source = 0;

    I2C_Master_ReadByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_INT_SOURCE, &int_source);

    while(int_source != 0)
    {
        if ( CHECK_BIT(int_source, 0) )
        {
            switch(bit)
            {
                case MMA8452Q_INT_SOURCE_SRC_DRDY:
                    SET_BIT(*mask, MMA8452Q_InterruptSource_DataReady);
                    break;
                case MMA8452Q_INT_SOURCE_SRC_FF_MT:
                    SET_BIT(*mask, MMA8452Q_InterruptSource_Freefall);
                    break;
                case MMA8452Q_INT_SOURCE_SRC_PULSE:
                    SET_BIT(*mask, MMA8452Q_InterruptSource_Pulse);
                    break;
                case MMA8452Q_INT_SOURCE_SRC_LNDPRT:
                    SET_BIT(*mask, MMA8452Q_InterruptSource_Orientation);
                    break;
                case MMA8452Q_INT_SOURCE_SRC_TRANS:
                    SET_BIT(*mask, MMA8452Q_InterruptSource_Transient);
                    break;
                case MMA8452Q_INT_SOURCE_SRC_ASLP:
                    SET_BIT(*mask, MMA8452Q_InterruptSource_AutoSleep);
                    break;
                default: HALT; // Should not get here
            }
        }

        bit++;
        int_source >>= 1;
    }
}

Std_ReturnType MMA8452Q_EnableInterrupt(MMA8452Q_InterruptSource_t interrupt)
{
    uint8_t ctrl_reg4 = 0;
    I2C_Master_ReadByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG4, &ctrl_reg4);

    switch(interrupt)
    {
        case MMA8452Q_InterruptSource_DataReady:
            SET_BIT(ctrl_reg4, MMA8452Q_CTRL_REG4_INT_EN_DRDY);
            break;
        case MMA8452Q_InterruptSource_Freefall:
            SET_BIT(ctrl_reg4, MMA8452Q_CTRL_REG4_INT_EN_FF_MT);
            break;
        case MMA8452Q_InterruptSource_Pulse:
            SET_BIT(ctrl_reg4, MMA8452Q_CTRL_REG4_INT_EN_PULSE);
            break;
        case MMA8452Q_InterruptSource_Orientation:
            SET_BIT(ctrl_reg4, MMA8452Q_CTRL_REG4_INT_EN_LNDPRT);
            break;
        case MMA8452Q_InterruptSource_Transient:
            SET_BIT(ctrl_reg4, MMA8452Q_CTRL_REG4_INT_EN_TRANS);
            break;
        case MMA8452Q_InterruptSource_AutoSleep:
            SET_BIT(ctrl_reg4, MMA8452Q_CTRL_REG4_INT_EN_ASLP);
            break;
        default:
            return Status_Invalid_Parameter;
    }

    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG4, ctrl_reg4);
    return Status_OK;
}

Std_ReturnType MMA8452Q_SetInterruptConfig(MMA8452Q_InterruptSource_t src, MMA8452Q_InterruptPin_t pin)
{
    uint8_t bit = 0;
    uint8_t ctrl_reg5 = 0;
    I2C_Master_ReadByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG5, &ctrl_reg5);

    switch(src)
    {
        case MMA8452Q_InterruptSource_DataReady:
            bit = MMA8452Q_CTRL_REG5_INT_CFG_DRDY;
            break;
        case MMA8452Q_InterruptSource_Freefall:
            bit = MMA8452Q_CTRL_REG5_INT_CFG_FF_MT;
            break;
        case MMA8452Q_InterruptSource_Pulse:
            bit = MMA8452Q_CTRL_REG5_INT_CFG_PULSE;
            break;
        case MMA8452Q_InterruptSource_Orientation:
            bit = MMA8452Q_CTRL_REG5_INT_CFG_LNDPRT;
            break;
        case MMA8452Q_InterruptSource_Transient:
            bit = MMA8452Q_CTRL_REG5_INT_CFG_TRANS;
            break;
        case MMA8452Q_InterruptSource_AutoSleep:
            bit = MMA8452Q_CTRL_REG5_INT_CFG_ASLP;
            break;
        default:
            return Status_Invalid_Parameter;
    }

    switch(pin)
    {
        case MMA8452Q_InterruptPin_INT1:
            SET_BIT(ctrl_reg5, bit);
            break;
        case MMA8452Q_InterruptPin_INT2:
            RESET_BIT(ctrl_reg5, bit);
            break;
        default:
            return Status_Invalid_Parameter;
    }

    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG5, ctrl_reg5);

    return Status_OK;
}

void MMA8452Q_SetFreefallMode(bool evt_x, bool evt_y, bool evt_z, bool latch)
{
    uint8_t ff_mt_cfg = 0;

    if(evt_x) SET_BIT(ff_mt_cfg, MMA8452Q_FF_MT_CFG_XTEFE);
    if(evt_y) SET_BIT(ff_mt_cfg, MMA8452Q_FF_MT_CFG_YTEFE);
    if(evt_z) SET_BIT(ff_mt_cfg, MMA8452Q_FF_MT_CFG_ZTEFE);
    if(latch) SET_BIT(ff_mt_cfg, MMA8452Q_FF_MT_CFG_ELE);

    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_FF_MT_CFG, ff_mt_cfg);
}

void MMA8452Q_SetFreefallThreshold(float threshold)
{
    uint8_t bVal = (uint8_t) MIN(threshold/0.063, 0x7F);
    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_FF_MT_THS, bVal);
}

void MMA8452Q_GetFreefallSource(uint8_t* src)
{
    I2C_Master_ReadByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_FF_MT_SRC, src);
}

void MMA8452Q_SetFreefallCount(uint8_t cnt)
{
    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_FF_MT_COUNT, cnt);
}

void MMA8452Q_SetTransientMode(bool evt_x, bool evt_y, bool evt_z, bool latch)
{
    uint8_t transient_cfg = 0;

    if(evt_x) SET_BIT(transient_cfg, MMA8452Q_TRANSIENT_CFG_XTEFE);
    if(evt_y) SET_BIT(transient_cfg, MMA8452Q_TRANSIENT_CFG_YTEFE);
    if(evt_z) SET_BIT(transient_cfg, MMA8452Q_TRANSIENT_CFG_ZTEFE);
    if(latch) SET_BIT(transient_cfg, MMA8452Q_TRANSIENT_CFG_ELE);

    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_TRANSIENT_CFG, transient_cfg);
}

void MMA8452Q_GetTransientSource(uint8_t* src)
{
    I2C_Master_ReadByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_TRANSIENT_SRC, src);
}

void MMA8452Q_SetTransientThreshold(float threshold)
{
    uint8_t bVal = (uint8_t) MIN(threshold/0.063, 0x7F);
    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_TRANSIENT_THS, bVal);
}

void MMA8452Q_SetTransientCount(uint8_t cnt)
{
    I2C_Master_WriteByteSync(MMA8452Q_I2C_ADDR, MMA8452Q_TRANSIENT_COUNT, cnt);
}
