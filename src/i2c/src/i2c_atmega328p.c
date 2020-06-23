#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <i2c.h>
#include <i2c_cfg.h>
#include <i2c_prv.h>
#include "bits.h"
#include "types.h"


#if I2C_MODE == I2C_MODE_SLAVE

ISR(TWI_vect)
{
    static int offset = -1;

    byte status = TW_STATUS;
    switch(status)
    {
        case TW_SR_SLA_ACK: // Received SLA+W
            offset = -1;
            break;
        case TW_ST_SLA_ACK: // Received SLA+R
        case TW_ST_DATA_ACK: // Sent data, received ACK
        case TW_ST_DATA_NACK: // Sent data, received NACK
            TWDR = I2C_Transmit(offset++);
            break;
        case TW_SR_DATA_ACK: // Received Data, sent ACK
            if (offset < 0) offset = TWDR;
            else I2C_Receive(offset++, TWDR);
            break;
        case TW_SR_STOP: // Received STOP or REPEATED START
            break;
        case TW_BUS_ERROR:
        default:
            // TODO : handle bus error, unhandled case
            HALT;
    }

    TWCR = BIT(TWIE) | BIT(TWINT) | BIT(TWEA) | BIT(TWEN);
}

#endif

void I2C_HAL_Init()
{
#if I2C_MODE == I2C_MODE_MASTER
    TWSR = 0;  // Prescaler = 1
    TWBR = ((F_CPU/F_I2C)-16)/2;
#elif I2C_MODE == I2C_MODE_SLAVE
    TWAR = I2C_SLAVE_ADRESS << 1;
    TWCR = BIT(TWIE) | BIT(TWEA) | BIT(TWINT) | BIT(TWEN);
#else
    #error I2C_MODE not set or incorrect
#endif
}

void I2C_HAL_EnablePeripheral()
{
    RESET_BIT(PRR, PRTWI);
}

bool I2C_HAL_TransmitReady()
{
    return IS_SET_BIT(TWCR, TWINT);
}

void I2C_HAL_StartCondition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!I2C_HAL_TransmitReady());

    switch(TW_STATUS)
    {
        case TW_START:
            // OK
            break;
        case TW_REP_START:
            HALT;
            break;
        default:
            HALT;
            break;
    }
}

void I2C_HAL_RestartCondition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!I2C_HAL_TransmitReady());

    switch(TW_STATUS)
    {
        case TW_START:
            HALT;
            break;
        case TW_REP_START:
            // OK
            break;
        default:
            HALT;
            break;
    }
}

void I2C_HAL_SlaveWrite(uint8_t slaveAddr)
{
    uint8_t status = I2C_HAL_Write( I2C_ADDR(slaveAddr, TW_WRITE) );
    switch(status)
    {
        case TW_MT_SLA_ACK:
            // OK
            break;
        case TW_MT_SLA_NACK:
            HALT;
            break;
        default:
            HALT;
            break;
    }
}

void I2C_HAL_SlaveRead(uint8_t slaveAddr)
{
    uint8_t status = I2C_HAL_Write( I2C_ADDR(slaveAddr, TW_READ) );
    switch(status)
    {
        case TW_MR_SLA_ACK:
            // OK
            break;
        case TW_MR_SLA_NACK:
            HALT;
            break;
        default:
            HALT;
            break;
    }
}

void I2C_HAL_StopCondition()
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWSTO);
    while(IS_SET_BIT(TWCR, TWSTO));
}

uint8_t I2C_HAL_Write(uint8_t data)
{
    TWDR = data;
    TWCR = BIT(TWINT) | BIT(TWEN);
    while (!I2C_HAL_TransmitReady());
    return TW_STATUS;
}

uint8_t I2C_HAL_ReadAck(void)
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWEA);
    while(!I2C_HAL_TransmitReady());

    switch(TW_STATUS)
    {
        case TW_MR_DATA_ACK:
            // OK
            break;
        case TW_MR_DATA_NACK:
            HALT;
            break;
        default:
            HALT;
            break;
    }

    return TWDR;
}

uint8_t I2C_HAL_ReadNak(void)
{
    TWCR = BIT(TWINT) | BIT(TWEN);
    while(!I2C_HAL_TransmitReady());

    switch(TW_STATUS)
    {
        case TW_MR_DATA_ACK:
            HALT;
            break;
        case TW_MR_DATA_NACK:
            // OK
            break;
        default:
            HALT;
            break;
    }

    return TWDR;
}
