#include "i2c.h"
#include "types.h"

/*
 * I2C Bus
 */

static char* error_strings[NUMBER_OF_I2C_ERRORS] = {
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

int i2c_bus_write(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length)
{
    if (self->drv->write == NULL_PTR) return -I2C_NOT_IMPLEMENTED;
    else return self->drv->write(self, addr, reg, data, length);
}

int i2c_bus_read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length)
{
    if (self->drv->read == NULL_PTR) return -I2C_NOT_IMPLEMENTED;
    else return self->drv->read(self, addr, reg, data, length);
}

/*
 * I2C Device
 */

int i2c_device_init(i2c_device_t dev, i2c_bus_t bus, uint8_t addr)
{
    dev->bus = bus;
    dev->addr = addr;
    return I2C_OK;
}

int i2c_device_write_byte(i2c_device_t self, uint8_t reg, uint8_t byte)
{
    return i2c_device_write_bytes(self, reg, &byte, 1);
}

int i2c_device_read_byte(i2c_device_t self, uint8_t reg, uint8_t *data)
{
    return i2c_device_read_bytes(self, reg, data, 1);
}

int i2c_device_write_bytes(i2c_device_t self, uint8_t reg, void *data, unsigned int length)
{
    return i2c_bus_write(self->bus, self->addr, reg, data, length);
}

int i2c_device_read_bytes(i2c_device_t self, uint8_t reg, void *data, unsigned int length)
{
    return i2c_bus_read(self->bus, self->addr, reg, data, length);
}

char* i2c_get_error_string(i2c_error_t errcode)
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
