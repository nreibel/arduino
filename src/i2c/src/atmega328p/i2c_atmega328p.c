#include "i2c.h"
#include "i2c_atmega328p.h"
#include "bits.h"
#include "types.h"
#include "os.h"

#include <avr/io.h>
#include <util/twi.h>

#if F_CPU != 16000000
#error "F_CPU not supported"
#endif

/*
 * Private defines
 */

#define I2C_ADDR(addr, rw) (((addr) << 1) | rw)
#define I2C_POLL_TIMEOUT 100 // in ms

/*
 * Private types definition
 */

typedef struct i2c_atmega328p_bus_prv_s {
    struct i2c_bus_prv_s super;
    bool fast_mode; // 400kHz
} * i2c_atmega328p_bus_t;

/*
 * Private functions declaration
 */

static int read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length);
static int write(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length);
static int init(i2c_bus_t self);

static int i2c_ll_wait_tx(unsigned int ms);
static int i2c_ll_wait_stop(unsigned int ms);
static int i2c_ll_start_condition();
static int i2c_ll_restart_condition();
static int i2c_ll_stop_condition();
static int i2c_ll_slave_write(uint8_t addr);
static int i2c_ll_slave_read(uint8_t addr);
static int i2c_ll_write(uint8_t data, uint8_t *status);
static int i2c_ll_read_ack(uint8_t *data);
static int i2c_ll_read_nack(uint8_t *data);

/*
 * Private data
 */

static struct i2c_driver_prv_s atmega328p_i2c_drv = {
    .init = init,
    .read = read,
    .write = write
};

/*
 * Exported data
 */

static struct i2c_atmega328p_bus_prv_s i2c_bus[NUMBER_OF_I2C_BUS];

/*
 * Public functions
 */

int i2c_atmega328p_init(bool fast_mode)
{
    int ret = i2c_bus_init(&i2c_bus[0].super, &atmega328p_i2c_drv);
    if (ret < 0) return ret;

    i2c_bus[0].fast_mode = fast_mode;

    return i2c_register_bus(&i2c_bus[0].super);
}

/*
 * Private functions
 */

static int init(i2c_bus_t self)
{
    i2c_atmega328p_bus_t bus = (i2c_atmega328p_bus_t) self;

    RESET_BIT(PRR, PRTWI);

    TWSR = 0; // Prescaler = 1
    TWCR = 0;

    // Formula is ((F_CPU/F_I2C)-16)/2;
    TWBR = bus->fast_mode ? 12 : 72;

    return I2C_OK;
}

static int write(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length)
{
    UNUSED(self);

    int ret = I2C_OK;
    unsigned int written = 0;
    uint8_t *bytes = data;

    ret = i2c_ll_start_condition();
    if (ret < 0) return ret;

    ret = i2c_ll_slave_write(addr);
    if (ret < 0) return ret;

    ret = i2c_ll_write(reg, NULL_PTR);
    if (ret < 0) return ret;

    for (unsigned int i = 0 ; i < length ; i++ )
    {
        ret = i2c_ll_write( bytes[i], NULL_PTR );
        if (ret < 0) return ret;
        else written += ret;
    }

    ret = i2c_ll_stop_condition();
    if (ret < 0) return ret;

    return written;
}

static int read(i2c_bus_t self, uint8_t addr, uint8_t reg, void *data, unsigned int length)
{
    UNUSED(self);

    int ret = I2C_OK;
    unsigned int read = 0;
    uint8_t *bytes = data;

    ret = i2c_ll_start_condition();
    if (ret < 0) return ret;

    ret = i2c_ll_slave_write(addr);
    if (ret < 0) return ret;

    ret = i2c_ll_write(reg, NULL_PTR);
    if (ret < 0) return ret;

    ret = i2c_ll_restart_condition();
    if (ret < 0) return ret;

    ret = i2c_ll_slave_read(addr);
    if (ret < 0) return ret;

    for(unsigned int i = 0 ; i < length-1 ; i++)
    {
        ret = i2c_ll_read_ack(bytes+i);
        if (ret < 0) return ret;
        else read += ret;
    }

    ret = i2c_ll_read_nack(bytes+length-1);
    if (ret < 0) return ret;
    else read += ret;

    ret = i2c_ll_stop_condition();
    if (ret < 0) return ret;

    return read;
}

/*
 * Low-Level API
 */

static int i2c_ll_wait_tx(unsigned int ms)
{
    time_t start = os_millis();

    do {
        time_t diff = os_millis() - start;
        if (diff > ms) return -I2C_TIMEOUT;
    }
    while( !IS_SET_BIT(TWCR, TWINT) );

    return I2C_OK;
}

static int i2c_ll_wait_stop(unsigned int ms)
{
    time_t start = os_millis();

    do {
        time_t diff = os_millis() - start;
        if (diff > ms) return -I2C_TIMEOUT;
    }
    while( IS_SET_BIT(TWCR, TWSTO) );

    return I2C_OK;
}

static int i2c_ll_start_condition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);

    switch( i2c_ll_wait_tx(I2C_POLL_TIMEOUT) )
    {
        case I2C_OK:
            // Do nothing
            break;

        case -I2C_TIMEOUT:
            return -I2C_TIMEOUT;

        default:
            return -I2C_SEQ_FAIL;
    }

    switch(TW_STATUS)
    {
        case TW_START:
            return I2C_OK;

        case TW_REP_START:
        default:
            TWCR = 0;
            return -I2C_FAIL;
    }
}

static int i2c_ll_restart_condition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);

    switch( i2c_ll_wait_tx(I2C_POLL_TIMEOUT) )
    {
        case I2C_OK:
            // Do nothing
            break;

        case -I2C_TIMEOUT:
            return -I2C_TIMEOUT;

        default:
            return -I2C_SEQ_FAIL;
    }

    switch(TW_STATUS)
    {
        case TW_REP_START:
            return I2C_OK;

        case TW_START:
        default:
            TWCR = 0;
            return -I2C_FAIL;
    }
}

static int i2c_ll_stop_condition()
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWSTO);

    switch(i2c_ll_wait_stop(I2C_POLL_TIMEOUT))
    {
        case I2C_OK:
            return I2C_OK;

        case -I2C_TIMEOUT:
            return -I2C_TIMEOUT;

        default:
            return -I2C_SEQ_FAIL;
    }
}

static int i2c_ll_slave_write(uint8_t addr)
{
    uint8_t status = 0;

    int ret = i2c_ll_write( I2C_ADDR(addr, TW_WRITE), &status );
    if (ret < 0) return ret;

    switch(status)
    {
        case TW_MT_SLA_ACK:
            return I2C_OK;

        case TW_MT_SLA_NACK:
        default:
            TWCR = 0;
            return -I2C_FAIL;
    }
}

static int i2c_ll_slave_read(uint8_t addr)
{
    uint8_t status = 0;

    int ret = i2c_ll_write( I2C_ADDR(addr, TW_READ), &status );
    if (ret < 0) return ret;

    switch(status)
    {
        case TW_MR_SLA_ACK:
            return I2C_OK;

        case TW_MR_SLA_NACK:
        default:
            TWCR = 0;
            return -I2C_FAIL;
    }
}

static int i2c_ll_write(uint8_t data, uint8_t *status)
{
    TWDR = data;
    TWCR = BIT(TWINT) | BIT(TWEN);

    switch( i2c_ll_wait_tx(I2C_POLL_TIMEOUT) )
    {
        case I2C_OK:
            // Do nothing
            break;

        case -I2C_TIMEOUT:
            return -I2C_TIMEOUT;

        default:
            return -I2C_SEQ_FAIL;
    }

    if (status != NULL_PTR)
        *status = TW_STATUS;

    return I2C_OK;
}

static int i2c_ll_read_ack(uint8_t *data)
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWEA);

    switch( i2c_ll_wait_tx(I2C_POLL_TIMEOUT) )
    {
        case I2C_OK:
            // Do nothing
            break;

        case -I2C_TIMEOUT:
            return -I2C_TIMEOUT;

        default:
            return -I2C_SEQ_FAIL;
    }

    switch(TW_STATUS)
    {
        case TW_MR_DATA_ACK:
            *data = TWDR;
            return 1;

        case TW_MR_DATA_NACK:
        default:
            TWCR = 0;
            return -I2C_FAIL;
    }
}

static int i2c_ll_read_nack(uint8_t *data)
{
    TWCR = BIT(TWINT) | BIT(TWEN);

    switch( i2c_ll_wait_tx(I2C_POLL_TIMEOUT) )
    {
        case I2C_OK:
            // Do nothing
            break;

        case -I2C_TIMEOUT:
            return -I2C_TIMEOUT;

        default:
            return -I2C_SEQ_FAIL;
    }

    switch(TW_STATUS)
    {
        case TW_MR_DATA_NACK:
            *data = TWDR;
            return 1;

        case TW_MR_DATA_ACK:
        default:
            TWCR = 0;
            return -I2C_FAIL;
    }
}
