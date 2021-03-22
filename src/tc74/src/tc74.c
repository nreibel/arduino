#include "tc74.h"
#include "tc74_prv.h"
#include "i2c_master.h"
#include "types.h"
#include "bits.h"

void tc74_init(tc74_t *self, uint8_t addr)
{
    self->i2c_addr = addr;
}

int tc74_set_standby(tc74_t *self, bool stdby)
{
    uint8_t regval = 0;

    I2C_Master_ReadByteSync(self->i2c_addr, TC74_REG_RWCR, &regval);

    if (stdby) SET_BIT(regval, 7);
    else RESET_BIT(regval, 7);

    return I2C_Master_WriteByteSync(self->i2c_addr, TC74_REG_RWCR, regval);
}

int tc74_read_temperature(tc74_t *self, int *temp)
{
    uint8_t reg = 0;
    int retval = I2C_Master_ReadByteSync(self->i2c_addr, TC74_REG_RTR, &reg);
    if (retval == 1)
    {
        *temp = (int) reg;
        if (reg >= 0x80) *temp -= 256; // two's complement
    }
    return retval;
}
