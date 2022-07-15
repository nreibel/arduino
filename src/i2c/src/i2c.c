#include "i2c_ll.h"
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
        return -I2C_ERR;

    self->instance = twi;
    self->fast_mode = fast_mode;

    return I2C_OK;
}

#if 0
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
        return -I2C_ERR_SEQ;

    return length;
}

int i2c_bus_read(i2c_bus_t self, uint8_t addr, uint8_t reg, void * data, unsigned int length)
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
        return -I2C_ERR_SEQ;

    return length;
}
#endif

int i2c_bus_transaction(i2c_bus_t self, uint8_t addr, const void * out, unsigned int wr, void * in, unsigned int rd, unsigned int timeout)
{
    int err = I2C_LL_OK;
    unsigned int nb_written = 0;
    unsigned int nb_read = 0;
    uint8_t * b_in = in;
    const uint8_t * b_out = out;

    if (wr > 0 && out == NULL_PTR)
        return -I2C_ERR_PARAM;

    if (rd > 0 && in == NULL_PTR)
        return -I2C_ERR_PARAM;

    enum {
        STATE_STA,
        STATE_SLA_W,
        STATE_WR,
        STATE_WR_COMPLETE,
        STATE_REP,
        STATE_SLA_R,
        STATE_RD,
        STATE_RD_NACK,
        STATE_STO,
        STATE_COMPLETE
    } state = STATE_STA;

    time_t tbeg = os_millis();

    while(os_millis() - tbeg < timeout)
    {
        switch(state)
        {
            case STATE_STA:
                err += i2c_ll_start_condition(self->instance);
                state = STATE_SLA_W;
                break;

            case STATE_SLA_W:
                err += i2c_ll_slave_write(self->instance, addr);
                state = STATE_WR;
                break;

            case STATE_WR:
                err += i2c_ll_write(self->instance, b_out[nb_written++]);
                if (nb_written >= wr) state = STATE_WR_COMPLETE;
                break;

            case STATE_WR_COMPLETE:
                if (rd > 0) state = STATE_REP;
                else state = STATE_STO;
                break;

            case STATE_REP:
                err += i2c_ll_restart_condition(self->instance);
                state = STATE_SLA_R;
                break;

            case STATE_SLA_R:
                err += i2c_ll_slave_read(self->instance, addr);
                state = STATE_RD;
                break;

            case STATE_RD:
                if (nb_read < rd - 1)
                    err += i2c_ll_read_ack(self->instance, &b_in[nb_read++]);
                else state = STATE_RD_NACK;
                break;

            case STATE_RD_NACK:
                err += i2c_ll_read_nack(self->instance, &b_in[nb_read++]);
                state = STATE_STO;
                break;

            case STATE_STO:
                err += i2c_ll_stop_condition(self->instance);
                state = STATE_COMPLETE;
                break;

            case STATE_COMPLETE:
                return I2C_OK;

            default:
                return -I2C_ERR_SEQ;
        }

        if (err != I2C_LL_OK)
            return -I2C_ERR_SEQ;
    }

    return -I2C_ERR_TIMEOUT;
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

#if 0
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
#endif

int i2c_device_transaction(i2c_device_t self, const void * out, unsigned int wr, void * in, unsigned int rd, unsigned int timeout)
{
    return i2c_bus_transaction(self->bus, self->addr, out, wr, in, rd, timeout);
}
