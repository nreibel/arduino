#include "pca954x.h"
#include "types.h"
#include "bits.h"
#include "i2c_master_prv.h"

int pca954x_init(pca954x_t *self, uint8_t i2c_addr)
{
    self->i2c_addr = i2c_addr;
    return 0;
}

int pca954x_disable(pca954x_t *self)
{
    I2C_Master_StartCondition();
    I2C_Master_SlaveWrite(self->i2c_addr);
    I2C_Master_Write(0, NULL_PTR);
    I2C_Master_StopCondition();

    return 0;
}

int pca954x_select(pca954x_t *self, uint8_t channel)
{
    uint8_t regval = BIT(2) | MASK(channel, 0x3);

    I2C_Master_StartCondition();
    I2C_Master_SlaveWrite(self->i2c_addr);
    I2C_Master_Write(regval, NULL_PTR);
    I2C_Master_StopCondition();

    return 0;
}