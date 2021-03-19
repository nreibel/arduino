#include "pca954x.h"
#include "pca954x_prv.h"
#include "types.h"
#include "bits.h"
#include "i2c_master_prv.h"

Std_ReturnType PCA954X_Init()
{
    return Status_OK;
}

Std_ReturnType PCA954X_Disable(uint8_t slaveAddr)
{
    I2C_Master_StartCondition();
    I2C_Master_SlaveWrite(slaveAddr);
    I2C_Master_Write(0, NULL_PTR);
    I2C_Master_StopCondition();

    return Status_OK;
}

Std_ReturnType PCA954X_SelectChannel(uint8_t slaveAddr, uint8_t channel)
{
    uint8_t regval = BIT(2) | MASK(channel, 0x3);

    I2C_Master_StartCondition();
    I2C_Master_SlaveWrite(slaveAddr);
    I2C_Master_Write(regval, NULL_PTR);
    I2C_Master_StopCondition();

    return Status_OK;
}
