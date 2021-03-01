#include "tc74.h"
#include "tc74_prv.h"
#include "i2c_master.h"
#include "types.h"
#include "bits.h"

static int twos_complement(uint8_t byte)
{
    if IS_SET_BIT(byte, 7) return byte - 256;
    else return byte;
}

Std_ReturnType TC74_Init()
{
    return Status_OK;
}

Std_ReturnType TC74_SetStandby(uint8_t slave, bool stdby)
{
    uint8_t regval = 0;

    I2C_Master_ReadByteSync(slave, TC74_REG_RWCR, &regval);

    if (stdby) SET_BIT(regval, 7);
    else RESET_BIT(regval, 7);

    I2C_Master_WriteByteSync(slave, TC74_REG_RWCR, regval);

    return Status_OK;
}

Std_ReturnType TC74_Read(uint8_t slave, uint8_t *temp)
{
    uint8_t regval = 0;

    I2C_Master_ReadByteSync(slave, TC74_REG_RTR, &regval);

    *temp = twos_complement(regval);

    return Status_OK;
}
