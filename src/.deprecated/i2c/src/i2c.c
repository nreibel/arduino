#include "i2c.h"
#include "i2c_ll.h"
#include "bits.h"
#include "types.h"
#include "os_cfg.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

struct i2c_bus_prv_t {
    tx_callback tx_cbk;
    rx_callback rx_cbk;
};

static struct i2c_bus_prv_t i2c_bus_instances[NUMBER_OF_I2C_BUSES];

ISR(TWI_vect)
{
    static bool init = FALSE;
    static unsigned int offset = 0;

    switch(TW_STATUS)
    {
        case TW_SR_SLA_ACK: // Received SLA+W
            // i2c_start_callback();
            offset = 0;
            init = TRUE;
            break;

        case TW_ST_SLA_ACK: // Received SLA+R
        case TW_ST_DATA_ACK: // Sent data, received ACK
        case TW_ST_DATA_NACK: // Sent data, received NACK
            if (i2c_bus_instances[I2C_BUS_0].tx_cbk != NULL_PTR)
            {
                TWDR = i2c_bus_instances[I2C_BUS_0].tx_cbk(I2C_BUS_0, offset);
            }
            else
            {
                TWDR = 0xFF;
            }
            offset++;
            break;

        case TW_SR_DATA_ACK: // Received Data, sent ACK
            if (init)
            {
                offset = TWDR;
                init = FALSE;
            }
            else if (i2c_bus_instances[I2C_BUS_0].rx_cbk != NULL_PTR)
            {
                i2c_bus_instances[I2C_BUS_0].rx_cbk(I2C_BUS_0, offset++, TWDR);
            }
            else
            {
                offset++;
            }
            break;

        case TW_SR_STOP: // Received STOP or REPEATED START
            // i2c_stop_callback();
            break;

        case TW_BUS_ERROR:
            // TODO : handle bus error
            break;

        default:
            HALT;
    }

    TWCR = BIT(TWIE) | BIT(TWINT) | BIT(TWEA) | BIT(TWEN);
}

int i2c_bus_init_slave(i2c_bus_t bus, uint8_t addr)
{
    switch(bus)
    {
        case I2C_BUS_0:
            i2c_bus_instances[bus].tx_cbk = NULL_PTR;
            i2c_bus_instances[bus].rx_cbk = NULL_PTR;

            RESET_BIT(PRR, PRTWI);

            TWAR = addr << 1;
            TWCR = BIT(TWIE) | BIT(TWEA) | BIT(TWINT) | BIT(TWEN);
            return 0;

        default:
            return -1;
    }

    return -1;
}

int i2c_bus_init_master(i2c_bus_t bus, bool fast_mode)
{
    switch(bus)
    {
        case I2C_BUS_0:
            RESET_BIT(PRR, PRTWI);

            TWSR = 0; // Prescaler = 1
            TWCR = 0;

            // Formula is ((F_CPU/F_I2C)-16)/2;
            #if F_CPU == 16000000
            TWBR = fast_mode ? 12 : 72;
            #else
            #error "F_CPU not supported"
            #endif

            return 0;

        default:
            return -1;
    }

    return -1;
}

int i2c_device_init(i2c_device_t dev, i2c_bus_t bus, uint8_t addr)
{
    dev->bus = bus;
    dev->addr = addr;
    return 0;
}

uint8_t i2c_device_get_addr(i2c_device_t self)
{
    return self->addr;
}

int i2c_device_write_byte(i2c_device_t self, uint8_t reg, uint8_t byte)
{
    return i2c_device_write_bytes(self, reg, &byte, 1);
}

int i2c_device_read_byte(i2c_device_t self, uint8_t reg, uint8_t *data)
{
    return i2c_device_read_bytes(self, reg, data, 1);
}

int i2c_device_write_bytes(i2c_device_t self, uint8_t reg, void *data, unsigned int length)
{
    int written = 0, retval = 0;
    uint8_t *bytes = TYPECAST(data, uint8_t*);

    if ( i2c_ll_start_condition() < 0 )
        return -1;

    if ( i2c_ll_slave_write(self->addr) < 0 )
        return -2;

    if ( i2c_ll_write(reg, NULL_PTR) < 0 )
        return -3;

    for (unsigned int i = 0 ; i < length ; i++ )
    {
        retval = i2c_ll_write( bytes[i], NULL_PTR );
        if (retval < 0) return -4;
        else written += retval;
    }

    if ( i2c_ll_stop_condition() < 0 )
        return -5;

    return written;
}

int i2c_device_read_bytes(i2c_device_t self, uint8_t reg, void *data, unsigned int length)
{
    int retval = 0, read = 0;
    uint8_t *bytes = data;

    if ( i2c_ll_start_condition() < 0)
        return -1;

    if ( i2c_ll_slave_write(self->addr) < 0 )
        return -2;

    if ( i2c_ll_write(reg, NULL_PTR) < 0 )
        return -3;

    if ( i2c_ll_restart_condition() < 0 )
        return -4;

    if ( i2c_ll_slave_read(self->addr) < 0 )
        return -5;

    for(unsigned int i = 0 ; i < length-1 ; i++)
    {
        retval = i2c_ll_read_ack( &bytes[i] );
        if (retval < 0) return -6;
        else read += retval;
    }

    retval = i2c_ll_read_nack( &bytes[length-1] );
    if (retval < 0) return -7;
    else read += retval;

    if ( i2c_ll_stop_condition() < 0 )
        return -8;

    return read;
}

int i2c_set_tx_callback(i2c_bus_t bus, tx_callback cbk)
{
    switch(bus)
    {
        case I2C_BUS_0:
            i2c_bus_instances[bus].tx_cbk = cbk;
            return 0;

        default:
            return -1;
    }

    return -1;
}
int i2c_set_rx_callback(i2c_bus_t bus, rx_callback cbk)
{
    switch(bus)
    {
        case I2C_BUS_0:
            i2c_bus_instances[bus].rx_cbk = cbk;
            return 0;

        default:
            return -1;
    }

    return -1;
}
