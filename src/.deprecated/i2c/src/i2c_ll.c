#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include "i2c_ll.h"
#include "os.h"
#include "os_cfg.h"
#include "bits.h"
#include "types.h"

#define I2C_ADDR(addr, rw) (((addr) << 1) | rw)

bool i2c_ll_tx_ready()
{
    return IS_SET_BIT(TWCR, TWINT);
}

int i2c_ll_start_condition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!i2c_ll_tx_ready());

    switch(TW_STATUS)
    {
        case TW_START:
            return 0;

        case TW_REP_START:
        default:
            TWCR = 0;
            return -1;
    }
}

int i2c_ll_restart_condition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!i2c_ll_tx_ready());

    switch(TW_STATUS)
    {
        case TW_REP_START:
            return 0;

        case TW_START:
        default:
            TWCR = 0;
            return -1;
    }
}

int i2c_ll_stop_condition()
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWSTO);
    while(IS_SET_BIT(TWCR, TWSTO));
    return 0;
}

int i2c_ll_slave_write(uint8_t addr)
{
    uint8_t status = 0;

    int ret = i2c_ll_write( I2C_ADDR(addr, TW_WRITE), &status );
    if (ret < 0) return ret;

    switch(status)
    {
        case TW_MT_SLA_ACK:
            return 0;

        case TW_MT_SLA_NACK:
        default:
            TWCR = 0;
            return -1;
    }
}

int i2c_ll_slave_read(uint8_t addr)
{
    uint8_t status = 0;

    int ret = i2c_ll_write( I2C_ADDR(addr, TW_READ), &status );
    if (ret < 0) return ret;

    switch(status)
    {
        case TW_MR_SLA_ACK:
            return 0;

        case TW_MR_SLA_NACK:
        default:
            TWCR = 0;
            return -1;
    }
}

int i2c_ll_write(uint8_t data, uint8_t *status)
{
    TWDR = data;
    TWCR = BIT(TWINT) | BIT(TWEN);
    while (!i2c_ll_tx_ready());
    if (status != NULL_PTR) *status = TW_STATUS;
    return 0;
}

int i2c_ll_read_ack(uint8_t *data)
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWEA);
    while(!i2c_ll_tx_ready());

    switch(TW_STATUS)
    {
        case TW_MR_DATA_ACK:
            if (data != NULL_PTR) *data = TWDR;
            return 1;

        case TW_MR_DATA_NACK:
        default:
            TWCR = 0;
            return -1;
    }
}

int i2c_ll_read_nack(uint8_t *data)
{
    TWCR = BIT(TWINT) | BIT(TWEN);
    while(!i2c_ll_tx_ready());

    switch(TW_STATUS)
    {
        case TW_MR_DATA_NACK:
            if (data != NULL_PTR) *data = TWDR;
            return 1;

        case TW_MR_DATA_ACK:
        default:
            TWCR = 0;
            return -1;
    }
}
