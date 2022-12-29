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

static const uint8_t * tx_ptr = NULL_PTR;
static unsigned int tx_len    = 0;
static unsigned int tx_cnt    = 0;

static uint8_t * rx_ptr    = NULL_PTR;
static unsigned int rx_len = 0;
static unsigned int rx_cnt = 0;

__attribute((weak))
void i2c_ll_callback(const i2c_ll_callback_args_t * args)
{
    UNUSED(args);
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
    const twi_t twi = TWI0;
    static uint8_t addr = 0;

    twcr_t twcr = {
        .bits = {
            .twie = 1,
            .twint = 1,
            .twea = 1,
            .twen = 1,
        }
    };

    switch(TW_STATUS)
    {
        // Received SLA+W
        case TW_SR_SLA_ACK:
        {
            rx_cnt = 0;
            addr = twi->twdr >> 1;

            const i2c_ll_callback_args_t args = {
                .twi     = twi,
                .address = addr,
                .event   = I2C_EVENT_RX_START,
                .size    = 0,
            };

            i2c_ll_callback(&args);
            break;
        }

        // Received DATA
        case TW_SR_DATA_ACK:
        case TW_SR_DATA_NACK:
        {
            if (rx_len <= 0)
            {
                const i2c_ll_callback_args_t args = {
                    .twi     = twi,
                    .address = addr,
                    .event   = I2C_EVENT_RX_MORE,
                    .size    = rx_cnt,
                };

                i2c_ll_callback(&args);
            }

            if (rx_len > 0)
            {
                *(rx_ptr++) = twi->twdr;
                rx_cnt++;
                rx_len--;
            }

            break;
        }

        // Receive STOP
        case TW_SR_STOP:
        {
            const i2c_ll_callback_args_t args = {
                .twi     = twi,
                .address = addr,
                .event   = I2C_EVENT_RX_COMPLETE,
                .size    = rx_cnt,
            };

            i2c_ll_callback(&args);
            break;
        }

        // Received SLA+R
        case TW_ST_SLA_ACK:
        {
            tx_cnt = 0;
            addr = twi->twdr >> 1;

            const i2c_ll_callback_args_t args = {
                .twi     = twi,
                .address = addr,
                .event   = I2C_EVENT_TX_START,
                .size    = 0,
            };

            i2c_ll_callback(&args);

            if (tx_len > 0)
            {
                twi->twdr = *(tx_ptr++);
                tx_cnt++;
                tx_len--;
            }
            else
            {
                twi->twdr = 0xFF;
            }

            break;
        }

        // Sending DATA
        case TW_ST_DATA_ACK:
        {
            if (tx_len <= 0)
            {
                const i2c_ll_callback_args_t args = {
                    .twi     = twi,
                    .address = addr,
                    .event   = I2C_EVENT_TX_MORE,
                    .size    = tx_cnt,
                };

                i2c_ll_callback(&args);
            }

            if (tx_len > 0)
            {
                twi->twdr = *(tx_ptr++);
                tx_cnt++;
                tx_len--;
            }
            else
            {
                twi->twdr = 0xFF;
            }

            break;
        }

        // Transmit STOP
        case TW_ST_DATA_NACK:
        {
            const i2c_ll_callback_args_t args = {
                .twi     = twi,
                .address = addr,
                .event   = I2C_EVENT_TX_COMPLETE,
                .size    = tx_cnt,
            };

            i2c_ll_callback(&args);
            break;
        }

        // Illegal start or stop condition
        case TW_BUS_ERROR:
        {
            const i2c_ll_callback_args_t args = {
                .twi     = twi,
                .address = 0,
                .event   = I2C_EVENT_BUS_ERROR,
                .size    = 0,
            };

            i2c_ll_callback(&args);
            break;
        }

        default:
        {
            const i2c_ll_callback_args_t args = {
                .twi     = twi,
                .address = 0,
                .event   = I2C_EVENT_SEQ_ERROR,
                .size    = 0,
            };

            i2c_ll_callback(&args);
            break;
        }
    }

    twi->twcr = twcr;
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

int i2c_ll_init_slave(twi_t twi, uint8_t addr, uint8_t mask)
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
    twi->twamr = ~mask << 1;
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
