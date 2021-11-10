#include "pca954x.h"
#include "types.h"
#include "bits.h"

int pca954x_init(pca954x_h self, i2c_bus_t bus, uint8_t addr)
{
    i2c_device_init(&self->dev, bus, addr);
    self->current_channel = 0xFF;
    return 0;
}

int pca954x_disable(pca954x_h self)
{
    uint8_t dev_addr = i2c_device_get_addr(&self->dev);

    self->current_channel = 0xFF;

    i2c_ll_start_condition();
    i2c_ll_slave_write(dev_addr);
    i2c_ll_write(0, NULL_PTR);
    i2c_ll_stop_condition();

    return 0;
}

int pca954x_select(pca954x_h self, uint8_t channel)
{
    uint8_t regval = BIT(2) | MASK(channel, 0x3);
    uint8_t dev_addr = i2c_device_get_addr(&self->dev);

    if (self->current_channel != channel)
    {
        i2c_ll_start_condition();
        i2c_ll_slave_write(dev_addr);
        i2c_ll_write(regval, NULL_PTR);
        i2c_ll_stop_condition();
    }

    self->current_channel = channel;

    return 0;
}
