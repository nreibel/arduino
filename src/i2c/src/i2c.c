#include "i2c.h"
#include "i2c_prv.h"
#include "bits.h"
#include "types.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

static i2c_t buses[NUMBER_OF_I2C_BUSES];

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
            if (buses[I2C_BUS_0].tx_cbk != NULL_PTR)
            {
                TWDR = buses[I2C_BUS_0].tx_cbk(I2C_BUS_0, offset);
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
            else if (buses[I2C_BUS_0].rx_cbk != NULL_PTR)
            {
                buses[I2C_BUS_0].rx_cbk(I2C_BUS_0, offset++, TWDR);
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

void i2c_bus_init_slave(i2c_bus_t bus, uint8_t addr)
{
    buses[bus].tx_cbk = NULL_PTR;
    buses[bus].rx_cbk = NULL_PTR;

    RESET_BIT(PRR, PRTWI);

    TWAR = addr << 1;
    TWCR = BIT(TWIE) | BIT(TWEA) | BIT(TWINT) | BIT(TWEN);
}

void i2c_set_tx_callback(i2c_bus_t bus, tx_callback cbk)
{
    buses[bus].tx_cbk = cbk;
}

void i2c_set_rx_callback(i2c_bus_t bus, rx_callback cbk)
{
    buses[bus].rx_cbk = cbk;
}
