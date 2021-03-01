#include "pca954x.h"
#include "pca954x_prv.h"
#include "types.h"
#include "bits.h"
#include "i2c_master.h"

Std_ReturnType PCA954X_Init()
{
    return Status_OK;
}

Std_ReturnType PCA954X_SetEnabled(uint8_t slaveAddr, bool enabled)
{
    uint8_t regval = 0;

    I2C_Master_ReadByteSync(slaveAddr, 0, &regval);

    if (enabled) SET_BIT(regval, 2);
    else RESET_BIT(regval, 2);

    I2C_Master_WriteByteSync(slaveAddr, 0, regval);

    return Status_OK;
}

Std_ReturnType PCA954X_SelectChannel(uint8_t slaveAddr, uint8_t channel)
{
    uint8_t regval = 0;

    I2C_Master_ReadByteSync(slaveAddr, 0, &regval);
    SET_BITS(regval, channel, 0x3);
    I2C_Master_WriteByteSync(slaveAddr, 0, regval);

    return Status_OK;
}
