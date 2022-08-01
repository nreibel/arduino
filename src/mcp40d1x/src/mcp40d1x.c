#include "mcp40d1x.h"
#include "os_cfg.h"
#include "os_mem.h"
#include "i2c.h"

#if OS_MALLOC
mcp40d1x_t mcp40d1x_create(i2c_bus_t bus, uint8_t addr)
{
    mcp40d1x_t self = os_malloc(sizeof(*self));

    if (self == NULL_PTR)
        goto exit;

    if (mcp40d1x_init(self, bus, addr) < 0)
        goto cleanup;

    return self;

    cleanup:
        os_free(self);

    exit:
        return NULL_PTR;
}

void mcp40d1x_destroy(mcp40d1x_t self)
{
    if (self != NULL_PTR)
    {
        os_free(self);
    }
}
#endif // OS_MALLOC

int mcp40d1x_init(mcp40d1x_t self, i2c_bus_t bus, uint8_t addr)
{
    int res = i2c_device_init(&self->dev, bus, addr);

    if (res != I2C_OK)
        return -MCP40D1x_ERR_BUS;

    return MCP40D1x_OK;
}

int mcp40d1x_write(mcp40d1x_t self, uint8_t raw, unsigned int timeout)
{
    const uint8_t data[] = {0, raw >> 1};
    int res = i2c_device_transaction(&self->dev, data, 2, NULL_PTR, 0, timeout);

    if (res != I2C_OK)
        return -MCP40D1x_ERR_BUS;

    return MCP40D1x_OK;
}

int mcp40d1x_read(mcp40d1x_t self, uint8_t * raw, unsigned int timeout)
{
    const uint8_t buf = 0;
    int res = i2c_device_transaction(&self->dev, &buf, 1, raw, 1, timeout);

    if (res != I2C_OK)
        return -MCP40D1x_ERR_BUS;

    *raw <<= 1;

    return MCP40D1x_OK;
}
