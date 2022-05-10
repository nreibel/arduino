#include "i2c_ll.h"
#include "i2c_cfg.h"
#include "types.h"
#include "bits.h"

#include "os.h"
#include "os_cfg.h"

#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#if F_CPU != 16000000
#error "F_CPU not supported"
#endif

/*
 * Private defines
 */

#define RD(addr) (((addr) << 1) | TW_READ)
#define WR(addr) (((addr) << 1) | TW_WRITE)

/*
 * Interrupt callbacks
 */

__attribute__((weak))
void i2c_ll_cbk_rx(twi_t bus, uint8_t *buffer, unsigned int length)
{
    UNUSED(bus);
    UNUSED(buffer);
    UNUSED(length);
}

__attribute__((weak))
uint8_t i2c_ll_cbk_tx(twi_t bus, unsigned int offset)
{
    UNUSED(bus);
    UNUSED(offset);

    return 0;
}

__attribute__((weak))
void i2c_ll_cbk_err(twi_t bus, uint8_t status)
{
    UNUSED(bus);
    UNUSED(status);
}

#define DBG_PRINT(x) // printf x

ISR(TWI_vect)
{
    static uint8_t buffer[I2C_RX_BUFFER_LENGTH];
    static unsigned int idx = 0;

    static enum {
        STATE_IDLE,
        STATE_RX,
        STATE_TX,
    } state = STATE_IDLE;

    const uint8_t status = TW_STATUS;

    switch(state)
    {
        case STATE_IDLE:
        {
            switch(status)
            {
                // Received SLA+W
                case TW_SR_SLA_ACK:
                    DBG_PRINT(("sla+w\r\n"));
                    idx = 0;
                    state = STATE_RX;
                    break;

                // Received SLA+R
                case TW_ST_SLA_ACK:
                {
                    DBG_PRINT(("sla+r\r\n"));
                    idx = 0;
                    uint8_t data = i2c_ll_cbk_tx(TWI0, idx++);
                    DBG_PRINT((">%02X\r\n", data));
                    TWI0->twdr = data;
                    state = STATE_TX;
                    break;
                }

                default:
                    i2c_ll_cbk_err(TWI0, status);
                    state = STATE_IDLE;
                    break;
            }

            break;
        }

        case STATE_RX:
        {
            switch(status)
            {
                // Received DATA
                case TW_SR_DATA_ACK:
                {
                    uint8_t rcvd = TWI0->twdr;
                    DBG_PRINT(("<%02X\r\n", rcvd));
                    if (idx < I2C_RX_BUFFER_LENGTH) buffer[idx++] = rcvd;
                    else i2c_ll_cbk_err(TWI0, status);
                    break;
                }

                // Receive STOP
                case TW_SR_STOP:
                {
                    DBG_PRINT(("sto\r\n"));
                    i2c_ll_cbk_rx(TWI0, buffer, idx);
                    state = STATE_IDLE;
                    break;
                }

                default:
                    i2c_ll_cbk_err(TWI0, status);
                    state = STATE_IDLE;
                    break;
            }

            break;
        }

        case STATE_TX:
        {
            switch(status)
            {
                // Sending DATA
                case TW_ST_DATA_ACK:
                {
                    uint8_t data = i2c_ll_cbk_tx(TWI0, idx++);
                    DBG_PRINT((">%02X\r\n", data));
                    TWI0->twdr = data;
                    break;
                }

                // Transmit STOP
                case TW_ST_DATA_NACK:
                    DBG_PRINT(("nack\r\n"));
                    state = STATE_IDLE;
                    break;

                default:
                    i2c_ll_cbk_err(TWI0, status);
                    state = STATE_IDLE;
                    break;
            }

            break;
        }

        default:
            HALT;
    }

    twcr_t twcr = {
        .bits = {
            .twie = 1,
            .twint = 1,
            .twea = 1,
            .twen = 1
        }
    };

    TWI0->twcr = twcr;
}

/*
 * Exported functions
 */

int i2c_ll_init_master(twi_t twi, bool fast_mode)
{
    power_twi_enable();

    // Reset device
    twi->twcr.reg = 0;

    // Prescaler = 1
    twi->twsr.bits.twps = I2C_LL_PSCL_1;

    // Formula is ((F_CPU/F_I2C)-16)/2;
    twi->twbr = fast_mode ? 12 /* 400kHz */ : 72 /* 100kHz */;

    return I2C_LL_OK;
}

int i2c_ll_init_slave(twi_t twi, uint8_t addr)
{
    if (addr > 0x7f)
        return -I2C_LL_SLAVE_ADDRESS;

    power_twi_enable();

    twcr_t twcr = {
        .bits = {
            .twie = 1,
            .twea = 1,
            .twint = 1,
            .twen = 1
        }
    };

    twi->twar = addr << 1;
    twi->twcr = twcr;

    return I2C_LL_OK;
}

int i2c_ll_wait_int(twi_t twi, unsigned int ms)
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

    int ret = i2c_ll_wait_int(twi, I2C_POLL_TIMEOUT);
    if (ret < 0) return ret;

    switch(TW_STATUS)
    {
        case TW_START:
            return I2C_LL_OK;

        default:
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

    int ret = i2c_ll_wait_int(twi, I2C_POLL_TIMEOUT);
    if (ret < 0) return ret;

    switch(TW_STATUS)
    {
        case TW_REP_START:
            return I2C_LL_OK;

        default:
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

    return i2c_ll_wait_stop(twi, I2C_POLL_TIMEOUT);
}

int i2c_ll_slave_write(twi_t twi, uint8_t addr)
{
    twcr_t twcr = {
        .bits = {
            .twint = 1,
            .twen = 1
        }
    };

    twi->twdr = WR(addr);
    twi->twcr = twcr;

    int ret = i2c_ll_wait_int(twi, I2C_POLL_TIMEOUT);
    if(ret < 0) return ret;

    switch(TW_STATUS)
    {
        case TW_MT_SLA_ACK:
            return I2C_LL_OK;

        default:
            return -I2C_LL_FAIL;
    }
}

int i2c_ll_slave_read(twi_t twi, uint8_t addr)
{
    twcr_t twcr = {
        .bits = {
            .twint = 1,
            .twen = 1
        }
    };

    twi->twdr = RD(addr);
    twi->twcr = twcr;

    int ret = i2c_ll_wait_int(twi, I2C_POLL_TIMEOUT);
    if(ret < 0) return ret;

    switch(TW_STATUS)
    {
        case TW_MR_SLA_ACK:
            return I2C_LL_OK;

        default:
            return -I2C_LL_FAIL;
    }
}

int i2c_ll_write(twi_t twi, uint8_t data)
{
    twcr_t twcr = {
        .bits = {
            .twint = 1,
            .twen = 1
        }
    };

    twi->twdr = data;
    twi->twcr = twcr;

    int ret = i2c_ll_wait_int(twi, I2C_POLL_TIMEOUT);
    if(ret < 0) return ret;

    switch(TW_STATUS)
    {
        // TODO
        case TW_MT_DATA_ACK:
        case TW_MT_DATA_NACK:
            return 1;

        default:
            return -I2C_LL_FAIL;
    }
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

    int ret = i2c_ll_wait_int(twi, I2C_POLL_TIMEOUT);
    if (ret < 0) return ret;

    switch(TW_STATUS)
    {
        case TW_MR_DATA_ACK:
            *data = twi->twdr;
            return 1;

        default:
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

    int ret = i2c_ll_wait_int(twi, I2C_POLL_TIMEOUT);
    if (ret < 0) return ret;

    switch(TW_STATUS)
    {
        case TW_MR_DATA_NACK:
            *data = twi->twdr;
            return 1;

        default:
            return -I2C_LL_FAIL;
    }
}
