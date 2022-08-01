#ifndef __MCP40D1X_API_H__
#define __MCP40D1X_API_H__

#include "types.h"
#include "i2c.h"
#include "os_cfg.h"

#define MCP40D1X_BASE_ADDR 0x2E

enum {
    MCP40D1x_OK,
    MCP40D1x_ERR,
    MCP40D1x_ERR_BUS,
};

typedef struct mcp40d1x_prv_s {
    struct i2c_device_prv_s dev;
} * mcp40d1x_t;

#if OS_MALLOC
mcp40d1x_t mcp40d1x_create(i2c_bus_t bus, uint8_t addr);
void mcp40d1x_destroy(mcp40d1x_t self);
#endif // OS_MALLOC

int mcp40d1x_init(mcp40d1x_t self, i2c_bus_t bus, uint8_t addr);
int mcp40d1x_write(mcp40d1x_t self, uint8_t val, unsigned int timeout);
int mcp40d1x_read(mcp40d1x_t self, uint8_t * val, unsigned int timeout);

#endif /* __MCP40D1X_API_H__ */
