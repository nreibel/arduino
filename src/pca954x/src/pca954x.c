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
 * Exported functions
 */


#if OS_MALLOC

pca954x_t pca954x_create(i2c_bus_t parent, uint8_t addr)
{
    pca954x_t self = os_malloc(sizeof(*self));

    if (self == NULL_PTR)
        goto exit;

    if (pca954x_init(self, parent, addr) < 0)
        goto cleanup;

    return self;

    cleanup:
        os_free(self);

    exit:
        return NULL_PTR;
}

void pca954x_destroy(pca954x_t self)
{
    if (self != NULL_PTR)
    {
        os_free(self);
    }
}

#endif // OS_MALLOC

int pca954x_init(pca954x_t self, i2c_bus_t parent, uint8_t addr)
{
    // Call superclass constructor
    int ret = i2c_device_init(&self->super, parent, addr);
    if (ret < 0) return ret;

    self->current = 0xFF;

    return I2C_OK;
}

int pca954x_select_channel(pca954x_t self, uint8_t bus)
{
    if (self->current == bus)
        return I2C_OK;

    uint8_t reg = BIT(PCA9544_BIT_EN) | MASK(bus, PCA9544_MSK_CH_SEL);
    int retval = i2c_device_write_bytes(&self->super, reg, NULL_PTR, 0);

    if (retval < 0)
    {
        self->current = 0xFF;
        return retval;
    }
    else
    {
        self->current = bus;
        return I2C_OK;
    }
}
