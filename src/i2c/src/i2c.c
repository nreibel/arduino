#include "i2c_ll.h"
#include "os.h"
#include "os_cfg.h"
#include "os_mem.h"
#include "i2c.h"
#include "i2c_cfg.h"
#include "types.h"

/*
 * Exported functions
 */

#if OS_MALLOC
i2c_device_t i2c_device_create(i2c_bus_t bus, uint8_t addr)
{
    i2c_device_t self = os_malloc(sizeof(*self));

    if (self != NULL_PTR)
    {
        self->bus = bus;
        self->addr = addr;
    }

    return self;
}

void i2c_device_destroy(i2c_device_t self)
{
    if (self != NULL_PTR)
    {
        os_free(self);
    }
}
#endif // OS_MALLOC

int i2c_bus_init(i2c_bus_t self, i2c_transfer_t transfer)
{
    self->transfer = transfer;
    return I2C_OK;
}

int i2c_device_init(i2c_device_t dev, i2c_bus_t bus, uint8_t addr)
{
    dev->bus = bus;
    dev->addr = addr;
    return I2C_OK;
}

int i2c_bus_transaction(i2c_bus_t self, uint8_t addr, const void * out, unsigned int wr, void * in, unsigned int rd)
{
    return self->transfer(self, addr, out, wr, in, rd);
}

int i2c_device_transaction(i2c_device_t self, const void * out, unsigned int wr, void * in, unsigned int rd)
{
    return i2c_bus_transaction(self->bus, self->addr, out, wr, in, rd);
}
