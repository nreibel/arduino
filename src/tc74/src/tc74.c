#include "tc74.h"
#include "i2c.h"
#include "types.h"
#include "bits.h"

/*
 * Private constants
 */

#define TC74_REG_RTR  0x0
#define TC74_REG_RWCR 0x1

/*
 * Public functions
 */

void tc74_init(tc74_t self, i2c_bus_t bus, uint8_t addr)
{
    i2c_device_init(&self->dev, bus, addr);
}

int tc74_set_standby(tc74_t self, bool stdby)
{
    uint8_t regval = 0;

    i2c_device_read_byte(&self->dev, TC74_REG_RWCR, &regval);

    if (stdby) SET_BIT(regval, 7);
    else RESET_BIT(regval, 7);

    return i2c_device_write_byte(&self->dev, TC74_REG_RWCR, regval);
}

int tc74_read_temperature(tc74_t self, int *temp)
{
    uint8_t reg = 0;
    int retval = i2c_device_read_byte(&self->dev, TC74_REG_RTR, &reg);
    if (retval == 1)
    {
        *temp = (int) reg;
        if (reg >= 0x80) *temp -= 256; // two's complement
    }
    return retval;
}
