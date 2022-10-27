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

static const uint8_t * tx_ptr;
static unsigned int tx_len;
static unsigned int tx_cnt;

static uint8_t * rx_ptr;
static unsigned int rx_len;
static unsigned int rx_cnt;

__attribute((weak))
void i2c_ll_callback(twi_t twi, i2c_event_t event, unsigned int size)
{
    UNUSED(twi);
    UNUSED(event);
    UNUSED(size);
}

int i2c_ll_set_rx_buffer(twi_t twi, void * buffer, unsigned int len)
{
    UNUSED(twi);

    rx_ptr = buffer;
    rx_len = len;
    return I2C_LL_OK;
}

int i2c_ll_set_tx_buffer(twi_t twi, const void * buffer, unsigned int len)
{
    UNUSED(twi);

    tx_ptr = buffer;
    tx_len = len;
    return I2C_LL_OK;
}

ISR(TWI_vect)
{
    // TODO : buffer overflow

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
                    i2c_ll_callback(TWI0, I2C_EVENT_RX_START, 0);
                    rx_cnt = 0;
                    state = STATE_RX;
                    break;

                // Received SLA+R
                case TW_ST_SLA_ACK:
                    i2c_ll_callback(TWI0, I2C_EVENT_TX_START, 0);
                    TWI0->twdr = *(tx_ptr++);
                    tx_cnt = 1;
                    state = STATE_TX;
                    break;

                // Illegal start or stop condition
                case TW_BUS_ERROR:
                    i2c_ll_callback(TWI0, I2C_EVENT_BUS_ERROR, 0);
                    break;

                default:
                    i2c_ll_callback(TWI0, I2C_EVENT_SEQ_ERROR, 0);
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
                case TW_SR_DATA_NACK:
                {
                    if (rx_cnt >= rx_len)
                        i2c_ll_callback(TWI0, I2C_EVENT_RX_MORE, rx_cnt);

                    *(rx_ptr++) = TWI0->twdr;
                    rx_cnt++;
                    break;
                }

                // Receive STOP
                case TW_SR_STOP:
                    i2c_ll_callback(TWI0, I2C_EVENT_RX_COMPLETE, rx_cnt);
                    state = STATE_IDLE;
                    break;

                default:
                    i2c_ll_callback(TWI0, I2C_EVENT_SEQ_ERROR, 0);
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
                    if (tx_cnt >= tx_len)
                        i2c_ll_callback(TWI0, I2C_EVENT_TX_MORE, tx_cnt);

                    TWI0->twdr = *(tx_ptr++);
                    tx_cnt++;
                    break;
                }

                // Transmit STOP
                case TW_ST_DATA_NACK:
                    i2c_ll_callback(TWI0, I2C_EVENT_TX_COMPLETE, tx_cnt);
                    state = STATE_IDLE;
                    break;

                default:
                    i2c_ll_callback(TWI0, I2C_EVENT_SEQ_ERROR, 0);
                    state = STATE_IDLE;
                    break;
            }

            break;
        }

        default:
            // TODO
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

int i2c_ll_init_master(twi_t twi, i2c_ll_clk_t clk)
{
    power_twi_enable();

    uint8_t twbr = 0;

    switch(clk)
    {
        case I2C_LL_100KHZ: twbr = 72; break;
        case I2C_LL_400KHZ: twbr = 12; break;
        default: return -I2C_LL_ERR_PARAMETER;
    }

    twi->twcr.reg = 0;
    twi->twsr.bits.twps = I2C_LL_PSCL_1;
    twi->twbr = twbr;

    return I2C_LL_OK;
}

int i2c_ll_init_slave(twi_t twi, uint8_t addr)
{
    if (addr > 0x7f)
        return -I2C_LL_ERR_PARAMETER;

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
        if (diff > ms) return -I2C_LL_ERR_TIMEOUT;
    }
    while(!twi->twcr.bits.twint);

    return I2C_LL_OK;
}

int i2c_ll_wait_stop(twi_t twi, unsigned int ms)
{
    time_t start = os_millis();

    do {
        time_t diff = os_millis() - start;
        if (diff > ms) return -I2C_LL_ERR_TIMEOUT;
    }
    while(twi->twcr.bits.twsto);

    return I2C_LL_OK;
}

int i2c_ll_reset_bus(twi_t twi)
{
    twi->twcr.reg = 0;
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
            return -I2C_LL_ERR_STATUS;
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
            return -I2C_LL_ERR_STATUS;
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
            return -I2C_LL_ERR_STATUS;
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
            return -I2C_LL_ERR_STATUS;
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
            return -I2C_LL_ERR_STATUS;
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
            return -I2C_LL_ERR_STATUS;
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
            return -I2C_LL_ERR_STATUS;
    }
}
