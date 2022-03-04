#include "i2c.h"
#include "i2c_ll.h"
#include "i2c_drv.h"
#include "i2c_cfg.h"
#include "bits.h"
#include "types.h"
#include "os.h"
#include "os_mem.h"

/*
 * Private functions declaration
 */

static int read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length);
static int write(i2c_bus_t self, uint8_t addr, uint8_t reg, const void *data, unsigned int length);
static int transaction(i2c_bus_t self, uint8_t addr, void *data, unsigned int wr, unsigned int rd, unsigned int delay);
static int init(i2c_bus_t self);

/*
 * Private data
 */

static struct i2c_driver_prv_s atmega_i2c_drv = {
    .init = init,
    .read = read,
    .write = write,
    .transaction = transaction
};

/*
 * Public functions
 */

#if OS_MALLOC

i2c_bus_t i2c_driver_create(twi_t twi, bool fast_mode)
{
    i2c_atmega_bus_t self = os_malloc(sizeof(*self));

    if (self == NULL_PTR)
        goto exit;

    if (i2c_driver_init(self, twi, fast_mode) != I2C_DRV_OK)
        goto cleanup;

    return &self->super;

    cleanup:
        os_free(self);

    exit:
        return NULL_PTR;
}

void i2c_driver_destroy(i2c_bus_t self)
{
    if (self != NULL_PTR)
    {
        os_free(self);
    }
}

#endif

int i2c_driver_init(i2c_atmega_bus_t bus, twi_t twi, bool fast_mode)
{
    bus->instance = twi;
    bus->fast_mode = fast_mode;

    if (i2c_bus_init(&bus->super, &atmega_i2c_drv) != I2C_OK)
        return -I2C_DRV_FAIL;

    return I2C_DRV_OK;
}

/*
 * Private functions
 */

static int init(i2c_bus_t self)
{
    if (self->drv != &atmega_i2c_drv)
        return -I2C_FAIL;

    i2c_atmega_bus_t bus = (i2c_atmega_bus_t) self;

    i2c_ll_init(bus->instance, bus->fast_mode);

    return I2C_OK;
}

static int transaction(i2c_bus_t self, uint8_t addr, void *data, const unsigned int wr, const unsigned int rd, unsigned int delay)
{
    if (self->drv != &atmega_i2c_drv)
        return -I2C_FAIL;

    i2c_atmega_bus_t bus = (i2c_atmega_bus_t) self;

    int err = 0;
    unsigned int written = 0;
    unsigned int read = 0;
    uint8_t *bytes = data;

    err += i2c_ll_start_condition(bus->instance);
    err += i2c_ll_slave_write(bus->instance, addr);

    while(written < wr)
        written += i2c_ll_write(bus->instance, bytes[written]);

    if (rd > 0)
    {
        if (delay > 0)
            os_wait(delay);

        err += i2c_ll_restart_condition(bus->instance);
        err += i2c_ll_slave_read(bus->instance, addr);

        while(read < rd-1)
            read += i2c_ll_read_ack(bus->instance, &bytes[read]);

        read += i2c_ll_read_nack(bus->instance, &bytes[read]);
    }

    err += i2c_ll_stop_condition(bus->instance);

    if (err != 0 || written != wr || read != rd)
        return -I2C_SEQ_FAIL;

    return rd;
}

static int write(i2c_bus_t self, uint8_t addr, uint8_t reg, const void *data, unsigned int length)
{
    if (self->drv != &atmega_i2c_drv)
        return -I2C_FAIL;

    i2c_atmega_bus_t bus = (i2c_atmega_bus_t) self;

    int err = 0;
    unsigned int written = 0;
    const uint8_t *bytes = data;

    err += i2c_ll_start_condition(bus->instance);
    err += i2c_ll_slave_write(bus->instance, addr);
    written += i2c_ll_write(bus->instance, reg);

    for(unsigned int i = 0 ; i < length ; i++)
        written += i2c_ll_write(bus->instance, bytes[i]);

    err += i2c_ll_stop_condition(bus->instance);

    if (err != 0 || written != length+1)
        return -I2C_SEQ_FAIL;

    return length;
}

static int read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length)
{
    if (self->drv != &atmega_i2c_drv)
        return -I2C_FAIL;

    i2c_atmega_bus_t bus = (i2c_atmega_bus_t) self;

    int err = 0;
    unsigned int written = 0;
    unsigned int read = 0;
    uint8_t *bytes = data;

    err += i2c_ll_start_condition(bus->instance);
    err += i2c_ll_slave_write(bus->instance, addr);
    written += i2c_ll_write(bus->instance, reg);
    err += i2c_ll_restart_condition(bus->instance);
    err += i2c_ll_slave_read(bus->instance, addr);

    while(read < length-1)
        read += i2c_ll_read_ack(bus->instance, &bytes[read]);

    read += i2c_ll_read_nack(bus->instance, &bytes[read]);
    err += i2c_ll_stop_condition(bus->instance);

    if (err != 0 || written != 1 || read != length)
        return -I2C_SEQ_FAIL;

    return length;
}
