#include "os.h"
#include "os_cfg.h"
#include "os_mem.h"
#include "i2c.h"
#include "i2c_cfg.h"
#include "types.h"

/*
 * I2C Bus
 */

int i2c_bus_init(i2c_bus_t self, twi_t twi, bool fast_mode)
{
    int err = 0;

    err += i2c_ll_init_master(twi, fast_mode);

    if (err != 0)
        return I2C_FAIL;

    self->instance = twi;
    self->fast_mode = fast_mode;

    return I2C_OK;
}

int i2c_bus_write(i2c_bus_t self, uint8_t addr, uint8_t reg, const void *data, unsigned int length)
{
    int err = 0;
    unsigned int written = 0;
    const uint8_t *bytes = data;

    err += i2c_ll_start_condition(self->instance);
    err += i2c_ll_slave_write(self->instance, addr);
    written += i2c_ll_write(self->instance, reg);

    for(unsigned int i = 0 ; i < length ; i++)
        written += i2c_ll_write(self->instance, bytes[i]);

    err += i2c_ll_stop_condition(self->instance);

    if (err != 0 || written != length+1)
        return -I2C_SEQ_FAIL;

    return length;
}

int i2c_bus_read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length)
{
    int err = 0;
    unsigned int written = 0;
    unsigned int read = 0;
    uint8_t *bytes = data;

    err += i2c_ll_start_condition(self->instance);
    err += i2c_ll_slave_write(self->instance, addr);
    written += i2c_ll_write(self->instance, reg);
    err += i2c_ll_restart_condition(self->instance);
    err += i2c_ll_slave_read(self->instance, addr);

    while(read < length-1)
        read += i2c_ll_read_ack(self->instance, &bytes[read]);

    read += i2c_ll_read_nack(self->instance, &bytes[read]);
    err += i2c_ll_stop_condition(self->instance);

    if (err != 0 || written != 1 || read != length)
        return -I2C_SEQ_FAIL;

    return length;
}

int i2c_bus_transaction(i2c_bus_t self, uint8_t addr, void *data, unsigned int wr, unsigned int rd, unsigned int delay)
{
    int err = 0;
    unsigned int nb_written = 0;
    unsigned int nb_read = 0;
    uint8_t *bytes = data;

    err += i2c_ll_start_condition(self->instance);
    err += i2c_ll_slave_write(self->instance, addr);

    while(nb_written < wr)
        nb_written += i2c_ll_write(self->instance, bytes[nb_written]);

    if (rd > 0)
    {
        if (delay > 0)
            os_wait(delay);

        err += i2c_ll_restart_condition(self->instance);
        err += i2c_ll_slave_read(self->instance, addr);

        while(nb_read < rd-1)
            nb_read += i2c_ll_read_ack(self->instance, &bytes[nb_read]);

        nb_read += i2c_ll_read_nack(self->instance, &bytes[nb_read]);
    }

    err += i2c_ll_stop_condition(self->instance);

    if (err != 0 || nb_written != wr || nb_read != rd)
        return -I2C_SEQ_FAIL;

    return rd;
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
