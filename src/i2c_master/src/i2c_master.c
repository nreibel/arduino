#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#include "i2c_master.h"
#include "i2c_master_cfg.h"
#include "i2c_master_prv.h"
#include "os.h"
#include "os_cfg.h"
#include "bits.h"
#include "types.h"

Std_ReturnType I2C_Master_Init()
{
    RESET_BIT(PRR, PRTWI);

    TWSR = 0;  // Prescaler = 1
    TWBR = ((F_CPU/I2C_MASTER_F_I2C)-16)/2;

    return Status_OK;
}

Std_ReturnType I2C_Master_ReadRegister(uint8_t addr, uint8_t reg, uint8_t *val)
{
    *val = reg;
    I2C_Master_ReadSync(addr, val, 1, 1, 0);
    return Status_OK;
}

Std_ReturnType I2C_Master_WriteRegister(uint8_t addr, uint8_t reg, uint8_t val)
{
    uint8_t buffer[2] = {reg, val};
    I2C_Master_WriteSync(addr, buffer, 2);
    return Status_OK;
}

Std_ReturnType I2C_Master_WriteSync(uint8_t addr, void *buffer, int len)
{
    I2C_Master_StartCondition();
    I2C_Master_SlaveWrite(addr);
    for (int i = 0 ; i < len ; i++ )
    {
        I2C_Master_Write( UINT8_PTR(buffer)[i] );
    }
    I2C_Master_StopCondition();

    return Status_OK;
}

Std_ReturnType I2C_Master_ReadSync(uint8_t addr, void *buffer, int writeLen, int readLen, int delay)
{
    I2C_Master_StartCondition();
    I2C_Master_SlaveWrite(addr);
    for(int i = 0 ; i < writeLen ; i++)
    {
        I2C_Master_Write(UINT8_PTR(buffer)[i]);
    }

    if (delay > 0)
    {
        I2C_Master_StopCondition();
        Os_Wait(delay);
        I2C_Master_StartCondition();
    }
    else
    {
        I2C_Master_RestartCondition();
    }

    I2C_Master_SlaveRead(addr);
    for(int i = 0 ; i < readLen-1 ; i++)
    {
        UINT8_PTR(buffer)[i] = I2C_Master_ReadAck();
    }
    UINT8_PTR(buffer)[readLen-1] = I2C_Master_ReadNak();
    I2C_Master_StopCondition();

    return Status_OK;
}

bool I2C_Master_TransmitReady()
{
    return IS_SET_BIT(TWCR, TWINT);
}

void I2C_Master_StartCondition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!I2C_Master_TransmitReady());

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

void I2C_Master_RestartCondition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!I2C_Master_TransmitReady());

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

void I2C_Master_SlaveWrite(uint8_t slaveAddr)
{
    uint8_t status = I2C_Master_Write( I2C_ADDR(slaveAddr, TW_WRITE) );
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

void I2C_Master_SlaveRead(uint8_t slaveAddr)
{
    uint8_t status = I2C_Master_Write( I2C_ADDR(slaveAddr, TW_READ) );
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

void I2C_Master_StopCondition()
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWSTO);
    while(IS_SET_BIT(TWCR, TWSTO));
}

uint8_t I2C_Master_Write(uint8_t data)
{
    TWDR = data;
    TWCR = BIT(TWINT) | BIT(TWEN);
    while (!I2C_Master_TransmitReady());
    return TW_STATUS;
}

uint8_t I2C_Master_ReadAck(void)
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWEA);
    while(!I2C_Master_TransmitReady());

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

uint8_t I2C_Master_ReadNak(void)
{
    TWCR = BIT(TWINT) | BIT(TWEN);
    while(!I2C_Master_TransmitReady());

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
