#include "os.h"
#include "os_mem.h"
#include "tc74.h"
#include "i2c.h"
#include "types.h"
#include "bits.h"

/*
 * Private constants
 */

static const uint8_t TC74_REG_RTR = 0x0;
static const uint8_t TC74_REG_RWCR = 0x1;

/*
 * Public functions
 */

#if OS_MALLOC
tc74_t tc74_create(void)
{
    tc74_t self = os_malloc(sizeof(*self));
    return self;
}

void tc74_destroy(tc74_t self)
{
    if (self != NULL_PTR)
    {
        os_free(self);
    }
}
#endif // OS_MALLOC

int tc74_init(tc74_t self, i2c_bus_t bus, uint8_t addr)
{
    int err = I2C_OK;

    err += i2c_device_init(&self->dev, bus, addr);

    if (err != I2C_OK)
        return -TC74_ERR_BUS;

    return TC74_OK;
}

int tc74_set_standby(tc74_t self, bool stdby)
{
    int err = I2C_OK;

    uint8_t cr = TC74_REG_RWCR;
    err += i2c_device_transaction(&self->dev, &cr, 1, &cr, 1);

    if (err != I2C_OK)
        return -TC74_ERR_BUS;

    if (stdby) SET_BIT(cr, 7);
    else RESET_BIT(cr, 7);

    uint8_t buf[2] = { TC74_REG_RWCR, cr };
    err += i2c_device_transaction(&self->dev, buf, 2, NULL_PTR, 0);

    if (err != I2C_OK)
        return -TC74_ERR_BUS;

    return TC74_OK;
}

int tc74_read_temperature(tc74_t self, int * temp)
{
    int err = I2C_OK;
    uint8_t buf = TC74_REG_RTR;

    err += i2c_device_transaction(&self->dev, &buf, 1, &buf, 1);

    if (err != I2C_OK)
        return -TC74_ERR_BUS;

    *temp = (int) buf;

    // two's complement
    if (buf >= 0x80)
        *temp -= 256;

    return TC74_OK;
}
