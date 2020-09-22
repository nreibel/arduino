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
    I2C_Master_ReadRegister(MMA8452Q_I2C_ADDR, MMA8452Q_WHO_AM_I, &whoami);
    if (whoami != MMA8452Q_DEVICE_ID) return Status_Not_OK;

#if READ_MODE_FAST == ON
    SET_BIT(ctrl_reg1, MMA8452Q_CTRL_REG1_F_READ);
#endif

    I2C_Master_WriteRegister(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG1, ctrl_reg1);

    return Status_OK;
}

void MMA8452Q_Reset()
{
    I2C_Master_WriteRegister(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG2, BIT(6) );
}

void MMA8452Q_GetStatus(uint8_t* status)
{
    I2C_Master_ReadRegister(MMA8452Q_I2C_ADDR, MMA8452Q_STATUS, status);
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
            return Status_Not_OK;
    }

    if (hpf) SET_BIT(xyz_data_cfg, MMA8452Q_XYZ_DATA_CFG_HPF_OUT);

    I2C_Master_WriteRegister(MMA8452Q_I2C_ADDR, MMA8452Q_XYZ_DATA_CFG, xyz_data_cfg);

    return Status_OK;
}

void MMA8452Q_SetStandby(bool standby)
{
    uint8_t ctrl_reg1 = 0;
    I2C_Master_ReadRegister(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG1, &ctrl_reg1);

    if (standby) RESET_BIT(ctrl_reg1, MMA8452Q_CTRL_REG1_ACTIVE);
    else SET_BIT(ctrl_reg1, MMA8452Q_CTRL_REG1_ACTIVE);

    I2C_Master_WriteRegister(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG1, ctrl_reg1);
}

void MMA8452Q_GetData(MMA8452Q_Data_t* buffer)
{
    uint8_t *b = UINT8_PTR(buffer);

    b[0] = MMA8452Q_OUT_X_MSB;
    I2C_Master_ReadSync(MMA8452Q_I2C_ADDR, buffer, 1, sizeof(MMA8452Q_Data_t), 0);

#if READ_MODE_FAST != ON
    buffer->acc_x = (b[0] << 4) | (b[1] >> 4);
    buffer->acc_y = (b[2] << 4) | (b[3] >> 4);
    buffer->acc_z = (b[4] << 4) | (b[5] >> 4);

    if (buffer->acc_x > 2048) buffer->acc_x -= 4096;
    if (buffer->acc_y > 2048) buffer->acc_y -= 4096;
    if (buffer->acc_z > 2048) buffer->acc_z -= 4096;
#endif
}

void MMA8452Q_GetInterruptStatus(uint8_t* status)
{
    I2C_Master_ReadRegister(MMA8452Q_I2C_ADDR, MMA8452Q_INT_SOURCE, status);
}

void MMA8452Q_EnableInterrupts(uint8_t mask)
{
    I2C_Master_WriteRegister(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG4, mask);
}

void MMA8452Q_SetInterruptsConfig(uint8_t config)
{
    I2C_Master_WriteRegister(MMA8452Q_I2C_ADDR, MMA8452Q_CTRL_REG5, config);
}

void MMA8452Q_SetTransientMode(uint8_t mask, bool latch)
{
    uint8_t transient_cfg = MASK(mask, 0x0E);
    if(latch) SET_BIT(transient_cfg, MMA8452Q_TRANSIENT_CFG_ELE);
    I2C_Master_WriteRegister(MMA8452Q_I2C_ADDR, MMA8452Q_TRANSIENT_CFG, transient_cfg);
}

void MMA8452Q_GetTransientSource(uint8_t* src)
{
    I2C_Master_ReadRegister(MMA8452Q_I2C_ADDR, MMA8452Q_TRANSIENT_SRC, src);
}

void MMA8452Q_SetTransientThreshold(float threshold)
{
    uint8_t bVal = (uint8_t) MIN(threshold/0.063, 0x7F);
    I2C_Master_WriteRegister(MMA8452Q_I2C_ADDR, MMA8452Q_TRANSIENT_THS, bVal);
}

void MMA8452Q_SetTransientCount(uint8_t cnt)
{
    I2C_Master_WriteRegister(MMA8452Q_I2C_ADDR, MMA8452Q_TRANSIENT_COUNT, cnt);
}
