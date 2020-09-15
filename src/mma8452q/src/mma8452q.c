#include "i2c_master.h"
#include "mma8452q.h"
#include "mma8452q_cfg.h"
#include "mma8452q_prv.h"
#include "types.h"
#include "bits.h"

void MMA8452Q_Init()
{
    // Enable accelerometer in fast mode
    byte buffer[2];
    buffer[0] = MMA8452Q_CTRL_REG1;
    buffer[1] = BIT(MMA8452Q_CTRL_REG1_ACTIVE) | BIT(MMA8452Q_CTRL_REG1_F_READ);
    I2C_Master_WriteSync(MMA8452Q_I2C_ADDR, buffer, 2);
}

void MMA8452Q_Read(MMA8452Q_Data_t* buffer)
{
    UINT8_PTR(buffer)[0] = MMA8452Q_STATUS;
    I2C_Master_ReadSync(MMA8452Q_I2C_ADDR, buffer, 1, sizeof(MMA8452Q_Data_t), 0);
}
