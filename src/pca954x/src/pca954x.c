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

    return PCA954X_OK;
}

int pca954x_select(pca954x_t self, uint8_t bus, unsigned int timeout)
{
    if (self->current == bus)
        return PCA954X_OK;

    uint8_t data = BIT(PCA9544_BIT_EN) | MASK(bus, PCA9544_MSK_CH_SEL);
    int retval = i2c_device_transaction(&self->super, &data, 1, NULL_PTR, 0, timeout);

    if (retval < 0)
    {
        self->current = 0xFF;
        return -PCA954X_ERR;
    }
    else
    {
        self->current = bus;
        return PCA954X_OK;
    }
}
