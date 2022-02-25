#include "os_cfg.h"
#include "os_mem.h"
#include "i2c.h"
#include "i2c_cfg.h"
#include "types.h"

/*
 * I2C Bus
 */

static const char * error_strings[NUMBER_OF_I2C_ERRORS] = {
    [I2C_OK] = "OK",
    [I2C_FAIL] = "Error",
    [I2C_TIMEOUT] = "Timeout",
    [I2C_SEQ_FAIL] = "Sequence error",
    [I2C_NOT_IMPLEMENTED] = "Feature not implemented",
};

int i2c_bus_init(i2c_bus_t self, i2c_driver_t drv)
{
    self->drv = drv;
    if (drv->init == NULL_PTR) return I2C_OK;
    else return drv->init(self);
}

int i2c_bus_write(i2c_bus_t self, uint8_t addr, uint8_t reg, const void *data, unsigned int length)
{
    if (self->drv->write == NULL_PTR) return -I2C_NOT_IMPLEMENTED;
    else return self->drv->write(self, addr, reg, data, length);
}

int i2c_bus_read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length)
{
    if (self->drv->read == NULL_PTR) return -I2C_NOT_IMPLEMENTED;
    else return self->drv->read(self, addr, reg, data, length);
}

int i2c_bus_transaction(i2c_bus_t self, uint8_t addr, void *data, unsigned int wr, unsigned int rd, unsigned int delay)
{
    if (self->drv->transaction == NULL_PTR) return -I2C_NOT_IMPLEMENTED;
    else return self->drv->transaction(self, addr, data, wr, rd, delay);
}

/*
 * I2C Device
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

int i2c_device_init(i2c_device_t dev, i2c_bus_t bus, uint8_t addr)
{
    dev->bus = bus;
    dev->addr = addr;
    return I2C_OK;
}

int i2c_device_write_byte(i2c_device_t self, uint8_t reg, const uint8_t byte)
{
    return i2c_device_write_bytes(self, reg, &byte, 1);
}

int i2c_device_read_byte(i2c_device_t self, uint8_t reg, uint8_t *data)
{
    return i2c_device_read_bytes(self, reg, data, 1);
}

int i2c_device_write_bytes(i2c_device_t self, uint8_t reg, const void *data, unsigned int length)
{
    return i2c_bus_write(self->bus, self->addr, reg, data, length);
}

int i2c_device_read_bytes(i2c_device_t self, uint8_t reg, void *data, unsigned int length)
{
    return i2c_bus_read(self->bus, self->addr, reg, data, length);
}

int i2c_device_transaction(i2c_device_t self, void *data, unsigned int wr, unsigned int rd, unsigned int delay)
{
    return i2c_bus_transaction(self->bus, self->addr, data, wr, rd, delay);
}

const char * i2c_get_error_string(int errcode)
{
    switch(-errcode)
    {
        case I2C_OK:
        case I2C_FAIL:
        case I2C_TIMEOUT:
        case I2C_SEQ_FAIL:
        case I2C_NOT_IMPLEMENTED:
            return error_strings[-errcode];
        default:
            return "Unknown";
    }
}
