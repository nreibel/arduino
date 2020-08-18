#include "i2c_slave.h"
#include "bits.h"
#include "types.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

ISR(TWI_vect)
{
    static int offset = -1;

    byte status = TW_STATUS;
    switch(status)
    {
        case TW_SR_SLA_ACK: // Received SLA+W
            I2C_Slave_StartCallback();
            offset = -1;
            break;
        case TW_ST_SLA_ACK: // Received SLA+R
        case TW_ST_DATA_ACK: // Sent data, received ACK
        case TW_ST_DATA_NACK: // Sent data, received NACK
            TWDR = I2C_Slave_TransmitCallback(offset++);
            break;
        case TW_SR_DATA_ACK: // Received Data, sent ACK
            if (offset < 0) offset = TWDR;
            else I2C_Slave_ReceiveCallback(offset++, TWDR);
            break;
        case TW_SR_STOP: // Received STOP or REPEATED START
            I2C_Slave_StopCallback();
            break;
        case TW_BUS_ERROR:
            // TODO : handle bus error
            break;
        default:
            HALT;
    }

    TWCR = BIT(TWIE) | BIT(TWINT) | BIT(TWEA) | BIT(TWEN);
}

Std_ReturnType I2C_Slave_Init(uint8_t addr)
{
    RESET_BIT(PRR, PRTWI);

    TWAR = addr << 1;
    TWCR = BIT(TWIE) | BIT(TWEA) | BIT(TWINT) | BIT(TWEN);

    return Status_OK;
}
