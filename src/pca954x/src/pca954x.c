#include "i2c.h"
#include "pca954x.h"
#include "bits.h"
#include "types.h"
#include "os.h"
#include "os_mem.h"

/*
 * Private constants
 */

#define PCA9544_BIT_EN     2
#define PCA9544_MSK_CH_SEL 0x3

/*
 * Private functions prototypes
 */

static int read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length);
static int write(i2c_bus_t self, uint8_t addr, uint8_t reg, const void *data, unsigned int length);
static int select_channel(pca9544_t self, pca954x_bus_t bus);

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


#if OS_MALLOC

pca9544_t pca9544_create(i2c_bus_t parent, uint8_t addr)
{
    pca9544_t self = os_malloc(sizeof(*self));

    if (self == NULL_PTR)
        goto exit;

    if (pca9544_init(self, parent, addr) < 0)
        goto cleanup;

    return self;

    cleanup:
        os_free(self);

    exit:
        return NULL_PTR;
}

void pca9544_destroy(pca9544_t self)
{
    if (self != NULL_PTR)
    {
        os_free(self);
    }
}

#endif // OS_MALLOC

int pca9544_init(pca9544_t self, i2c_bus_t parent, uint8_t addr)
{
    // Call superclass constructor
    int ret = i2c_device_init(&self->super, parent, addr);
    if (ret < 0) return ret;

    self->current = NULL_PTR;

    for (uint8_t i = 0 ; i < PCA9544_NBR_OF_CHANNELS ; i++)
    {
        // Init and register each mux channel
        i2c_bus_init(&self->bus[i].super, &pca954x_i2c_drv);
        self->bus[i].dev = self;
        self->bus[i].channel_id = i;
    }

    return I2C_OK;
}

i2c_bus_t pca9544_get_bus(pca9544_t self, unsigned int bus)
{
    if (bus >= PCA9544_NBR_OF_CHANNELS)
        return NULL_PTR;

    return &self->bus[bus].super;
}

/*
 * Private functions
 */

static int read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length)
{
    pca954x_bus_t ch = (pca954x_bus_t) self;
    pca9544_t dev = ch->dev;
    i2c_bus_t bus = dev->super.bus;

    if (dev->current != ch)
    {
        int retval = select_channel(dev, ch);
        if (retval < 0) return retval;
    }

    return i2c_bus_read(bus, addr, reg, data, length);
}

static int write(i2c_bus_t self, uint8_t addr, uint8_t reg, const void *data, unsigned int length)
{
    pca954x_bus_t ch = (pca954x_bus_t) self;
    pca9544_t dev = ch->dev;
    i2c_bus_t bus = dev->super.bus;

    if (dev->current != ch)
    {
        int retval = select_channel(dev, ch);
        if (retval < 0) return retval;
    }

    return i2c_bus_write(bus, addr, reg, data, length);
}

static int select_channel(pca9544_t self, pca954x_bus_t bus)
{
    uint8_t reg = BIT(PCA9544_BIT_EN) | MASK(bus->channel_id, PCA9544_MSK_CH_SEL);
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
