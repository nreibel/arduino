#include "i2c_ll.h"
#include "i2c_cfg.h"
#include "types.h"

#include "os.h"
#include "os_cfg.h"

#include <avr/power.h>
#include <util/twi.h>

#if F_CPU != 16000000
#error "F_CPU not supported"
#endif

/*
 * Private defines
 */

#define I2C_ADDR(addr, rw) (((addr) << 1) | rw)

/*
 * Exported functions
 */

int i2c_ll_init(twi_t twi, bool fast_mode)
{
    power_twi_enable();

    // Reset device
    twi->twcr.reg = 0;

    // Prescaler = 1
    twi->twsr = 0;

    // Formula is ((F_CPU/F_I2C)-16)/2;
    twi->twbr = fast_mode ? 12 /* 400kHz */ : 72 /* 100kHz */;

    return I2C_LL_OK;
}

int i2c_ll_wait_tx(twi_t twi, unsigned int ms)
{
    time_t start = os_millis();

    do {
        time_t diff = os_millis() - start;
        if (diff > ms) return -I2C_LL_TIMEOUT;
    }
    while(!twi->twcr.bits.twint);

    return I2C_LL_OK;
}

int i2c_ll_wait_stop(twi_t twi, unsigned int ms)
{
    time_t start = os_millis();

    do {
        time_t diff = os_millis() - start;
        if (diff > ms) return -I2C_LL_TIMEOUT;
    }
    while(twi->twcr.bits.twsto);

    return I2C_LL_OK;
}

int i2c_ll_start_condition(twi_t twi)
{
    twcr_t twcr = {
        .bits = {
            .twint = 1,
            .twen = 1,
            .twsta = 1
        }
    };

    twi->twcr = twcr;

    switch( i2c_ll_wait_tx(twi, I2C_POLL_TIMEOUT) )
    {
        case I2C_LL_OK: /* Do nothing */ break;
        case -I2C_LL_TIMEOUT: return -I2C_LL_TIMEOUT;
        default: return -I2C_LL_SEQ_FAIL;
    }

    switch(TW_STATUS)
    {
        case TW_START:
            return I2C_LL_OK;

        case TW_REP_START:
        default:
            twi->twcr.reg = 0;
            return -I2C_LL_FAIL;
    }
}

int i2c_ll_restart_condition(twi_t twi)
{
    twcr_t twcr = {
        .bits = {
            .twint = 1,
            .twen = 1,
            .twsta = 1
        }
    };

    twi->twcr = twcr;

    switch( i2c_ll_wait_tx(twi, I2C_POLL_TIMEOUT) )
    {
        case I2C_LL_OK: /* Do nothing */ break;
        case -I2C_LL_TIMEOUT: return -I2C_LL_TIMEOUT;
        default: return -I2C_LL_SEQ_FAIL;
    }

    switch(TW_STATUS)
    {
        case TW_REP_START:
            return I2C_LL_OK;

        case TW_START:
        default:
            twi->twcr.reg = 0;
            return -I2C_LL_FAIL;
    }
}

int i2c_ll_stop_condition(twi_t twi)
{
    twcr_t twcr = {
        .bits = {
            .twint = 1,
            .twen = 1,
            .twsto = 1
        }
    };

    twi->twcr = twcr;

    switch(i2c_ll_wait_stop(twi, I2C_POLL_TIMEOUT))
    {
        case I2C_LL_OK:       return I2C_LL_OK;
        case -I2C_LL_TIMEOUT: return -I2C_LL_TIMEOUT;
        default: return -I2C_LL_SEQ_FAIL;
    }
}

int i2c_ll_slave_write(twi_t twi, uint8_t addr)
{
    uint8_t status = 0;

    int ret = i2c_ll_write(twi, I2C_ADDR(addr, TW_WRITE), &status );
    if (ret < 0) return ret;

    switch(status)
    {
        case TW_MT_SLA_ACK:
            return I2C_LL_OK;

        case TW_MT_SLA_NACK:
        default:
            twi->twcr.reg = 0;
            return -I2C_LL_FAIL;
    }
}

int i2c_ll_slave_read(twi_t twi, uint8_t addr)
{
    uint8_t status = 0;

    int ret = i2c_ll_write(twi, I2C_ADDR(addr, TW_READ), &status );
    if (ret < 0) return ret;

    switch(status)
    {
        case TW_MR_SLA_ACK:
            return I2C_LL_OK;

        case TW_MR_SLA_NACK:
        default:
            twi->twcr.reg = 0;
            return -I2C_LL_FAIL;
    }
}

int i2c_ll_write(twi_t twi, uint8_t data, uint8_t *status)
{
    twcr_t twcr = {
        .bits = {
            .twint = 1,
            .twen = 1
        }
    };

    twi->twdr = data;
    twi->twcr = twcr;

    switch( i2c_ll_wait_tx(twi, I2C_POLL_TIMEOUT) )
    {
        case I2C_LL_OK:
            // Do nothing
            break;

        case -I2C_LL_TIMEOUT:
            return -I2C_LL_TIMEOUT;

        default:
            return -I2C_LL_SEQ_FAIL;
    }

    if (status != NULL_PTR)
        *status = TW_STATUS;

    return 1;
}

int i2c_ll_read_ack(twi_t twi, uint8_t *data)
{
    twcr_t twcr = {
        .bits = {
            .twint = 1,
            .twen = 1,
            .twea = 1
        }
    };

    twi->twcr = twcr;

    switch( i2c_ll_wait_tx(twi, I2C_POLL_TIMEOUT) )
    {
        case I2C_LL_OK: /* Do nothing */ break;
        case -I2C_LL_TIMEOUT: return -I2C_LL_TIMEOUT;
        default: return -I2C_LL_SEQ_FAIL;
    }

    switch(TW_STATUS)
    {
        case TW_MR_DATA_ACK:
            *data = twi->twdr;
            return 1;

        case TW_MR_DATA_NACK:
        default:
            twi->twcr.reg = 0;
            return -I2C_LL_FAIL;
    }
}

int i2c_ll_read_nack(twi_t twi, uint8_t *data)
{
    twcr_t twcr = {
        .bits = {
            .twint = 1,
            .twen = 1
        }
    };

    twi->twcr = twcr;

    switch( i2c_ll_wait_tx(twi, I2C_POLL_TIMEOUT) )
    {
        case I2C_LL_OK:
            // Do nothing
            break;

        case -I2C_LL_TIMEOUT:
            return -I2C_LL_TIMEOUT;

        default:
            return -I2C_LL_SEQ_FAIL;
    }

    switch(TW_STATUS)
    {
        case TW_MR_DATA_NACK:
            *data = twi->twdr;
            return 1;

        case TW_MR_DATA_ACK:
        default:
            twi->twcr.reg = 0;
            return -I2C_LL_FAIL;
    }
}
