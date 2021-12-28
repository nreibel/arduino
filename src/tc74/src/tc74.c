#include "os.h"
#include "os_mem.h"
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

#if OS_MALLOC
tc74_t tc74_create(i2c_bus_t bus, uint8_t addr)
{
    tc74_t instance = os_malloc(sizeof(*instance));
    if (instance != NULL_PTR && tc74_init(instance, bus, addr) >= 0)
        return instance;
    else
        return NULL_PTR;
}

void tc74_destroy(tc74_t self)
{
    os_free(self);
}
#endif // OS_MALLOC

int tc74_init(tc74_t self, i2c_bus_t bus, uint8_t addr)
{
    return i2c_device_init(&self->dev, bus, addr);
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
