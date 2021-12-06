#include "i2c.h"
#include "pca954x.h"
#include "bits.h"
#include "types.h"

/*
 * Private constants
 */

#define BIT_ENABLE 2
#define MSK_CH_SEL 0x3

/*
 * Private functions prototypes
 */

static int read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length);
static int write(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length);
static int select_channel(pca954x_t self, pca954x_bus_t bus);

/*
 * Private data
 */

static struct i2c_driver_prv_s pca954x_i2c_drv = {
    .init = NULL_PTR,
    .read = read,
    .write = write
};

/*
 * Public functions
 */

int pca9544_init(pca954x_t self, i2c_bus_t parent, uint8_t addr)
{
    return  pca954x_init(self, parent, addr, 4);
}

int pca954x_init(pca954x_t self, i2c_bus_t parent, uint8_t addr, unsigned int nbr_of_channels)
{
    // Call superclass constructor
    int ret = i2c_device_init(&self->super, parent, addr);
    if (ret < 0) return ret;

    self->nbr_of_channels = nbr_of_channels;
    self->current = NULL_PTR;

    for (unsigned int i = 0 ; i < nbr_of_channels ; i++)
    {
        // Init and register each mux channel
        i2c_bus_init(&self->bus[i].super, &pca954x_i2c_drv);
        i2c_register_bus(&self->bus[i].super);
        self->bus[i].dev = self;
        self->bus[i].channel_id = i;
    }

    return I2C_OK;
}

i2c_bus_t pca954x_get_bus(pca954x_t self, unsigned int bus)
{
    if (bus >= self->nbr_of_channels) return NULL_PTR;
    else return (i2c_bus_t) &self->bus[bus];
}

/*
 * Private functions
 */

static int read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length)
{
    pca954x_bus_t ch = (pca954x_bus_t) self;
    pca954x_t dev = ch->dev;
    i2c_bus_t bus = dev->super.bus;

    if (dev->current != ch)
    {
        int retval = select_channel(dev, ch);
        if (retval < 0) return retval;
    }

    return i2c_bus_read(bus, addr, reg, data, length);
}

static int write(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length)
{
    pca954x_bus_t ch = (pca954x_bus_t) self;
    pca954x_t dev = ch->dev;
    i2c_bus_t bus = dev->super.bus;

    if (dev->current != ch)
    {
        int retval = select_channel(dev, ch);
        if (retval < 0) return retval;
    }

    return i2c_bus_write(bus, addr, reg, data, length);
}

static int select_channel(pca954x_t self, pca954x_bus_t bus)
{
    uint8_t reg = BIT(BIT_ENABLE) | MASK(bus->channel_id, MSK_CH_SEL);
    int retval = i2c_device_write_bytes(&self->super, reg, NULL_PTR, 0);
    if (retval < 0)
    {
        self->current = NULL_PTR;
        return retval;
    }
    else
    {
        self->current = bus;
        return I2C_OK;
    }
}
