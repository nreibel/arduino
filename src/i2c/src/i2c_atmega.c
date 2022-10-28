#include "i2c.h"
#include "os.h"
#include "types.h"

static int atmega_set_timeout(i2c_atmega_t self, unsigned int timeout)
{
    self->timeout = timeout;
    return I2C_OK;
}

static int atmega_transfer(void * handle, uint8_t addr, const void * out, unsigned int wr, void * in, unsigned int rd)
{
    i2c_atmega_t self = (i2c_atmega_t) handle;
    int err = I2C_LL_OK;

    unsigned int nb_written = 0;
    unsigned int nb_read = 0;
    uint8_t * b_in = in;
    const uint8_t * b_out = out;

    // TODO handle read only operations
    if (wr < 1 || out == NULL_PTR)
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

    while(os_millis() - tbeg < self->timeout)
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
                nb_written += i2c_ll_write(self->instance, b_out[nb_written]);
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
                    nb_read += i2c_ll_read_ack(self->instance, &b_in[nb_read]);
                else state = STATE_RD_NACK;
                break;

            case STATE_RD_NACK:
                nb_read += i2c_ll_read_nack(self->instance, &b_in[nb_read]);
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
        {
            i2c_ll_reset_bus(self->instance);
            return -I2C_ERR_SEQ;
        }
    }

    if (nb_read != rd && nb_written != wr)
        return -I2C_ERR_SEQ;

    return -I2C_ERR_TIMEOUT;
}

/*
 * Exported functions
 */

int i2c_atmega_init(i2c_atmega_t self, twi_t twi, bool fast_mode, unsigned int timeout)
{
    int err = 0;

    err += i2c_ll_init_master(twi, fast_mode ? I2C_LL_400KHZ : I2C_LL_100KHZ);

    if (err < 0)
        return -I2C_ERR;

    i2c_bus_init(&self->super, atmega_transfer);
    self->set_timeout = atmega_set_timeout;
    self->instance = twi;
    self->timeout = timeout;

    return I2C_OK;
}
