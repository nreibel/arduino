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

static int pca954x_transfer(void * handle, uint8_t addr, const void * out, unsigned int wr, void * in, unsigned int rd)
{
    pca954x_bus_t self = (pca954x_bus_t) handle;

    if (self->parent->current != self->channel)
    {
        pca954x_select(self->parent, self->channel);
    }

    return i2c_bus_transaction(self->parent->super.bus, addr, out, wr, in, rd);
}

/*
 * Exported functions
 */

int pca954x_bus_init(pca954x_bus_t self, pca954x_t parent, uint8_t channel)
{
    i2c_bus_init(&self->super, pca954x_transfer);
    self->parent = parent;
    self->channel = channel;
    return PCA954X_OK;
}

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

    return PCA954X_OK;
}

int pca954x_select(pca954x_t self, uint8_t channel)
{
    if (self->current == channel)
        return PCA954X_OK;

    uint8_t data = BIT(PCA9544_BIT_EN) | MASK(channel, PCA9544_MSK_CH_SEL);
    int retval = i2c_device_transaction(&self->super, &data, 1, NULL_PTR, 0);

    if (retval < 0)
    {
        self->current = 0xFF;
        return retval;
    }
    else
    {
        self->current = channel;
        return PCA954X_OK;
    }
}
