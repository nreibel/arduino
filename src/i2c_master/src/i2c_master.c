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
    TWCR = 0;

    return Status_OK;
}

int I2C_Master_ReadByteSync(uint8_t addr, uint8_t reg, uint8_t *val)
{
    return I2C_Master_ReadSync(addr, reg, val, 1);
}

int I2C_Master_WriteByteSync(uint8_t addr, uint8_t reg, uint8_t val)
{
    return I2C_Master_WriteSync(addr, reg, &val, 1);
}

int I2C_Master_WriteSync(uint8_t addr, uint8_t reg, buffer_t buffer, int len)
{
    Std_ReturnType retval = Status_Not_OK;

    retval = I2C_Master_StartCondition();
    if (retval != 0) return -1;

    retval = I2C_Master_SlaveWrite(addr);
    if (retval != Status_OK) return -2;

    retval = I2C_Master_Write(reg, NULL_PTR);
    if (retval != Status_OK) return -3;

    for (int i = 0 ; i < len ; i++ )
    {
        retval = I2C_Master_Write( UINT8_PTR(buffer)[i], NULL_PTR );
        if (retval != Status_OK) return -4;
    }

    retval = I2C_Master_StopCondition();
    if (retval != Status_OK) return -5;

    return Status_OK;
}

int I2C_Master_ReadSync(uint8_t addr, uint8_t reg, buffer_t buffer, int len)
{
    int retval = 0;
    uint8_t *ptr = UINT8_PTR(buffer);

    if ( I2C_Master_StartCondition() != Status_OK)
        return -1;

    if ( I2C_Master_SlaveWrite(addr) != Status_OK)
        return -2;

    if ( I2C_Master_Write(reg, NULL_PTR) != Status_OK)
        return -3;

    if ( I2C_Master_RestartCondition() != Status_OK)
        return -4;

    if ( I2C_Master_SlaveRead(addr) != Status_OK)
        return -5;

    for(int i = 0 ; i < len-1 ; i++)
    {
        if ( I2C_Master_ReadAck( &ptr[i] ) != Status_OK)
            return -6;
        retval++;
    }

    if ( I2C_Master_ReadNak( &ptr[len-1] ) != Status_OK)
        return -7;
    retval++;

    if ( I2C_Master_StopCondition() != Status_OK)
        return -8;

    return retval;
}

bool I2C_Master_TransmitReady()
{
    return IS_SET_BIT(TWCR, TWINT);
}

Std_ReturnType I2C_Master_StartCondition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!I2C_Master_TransmitReady());

    switch(TW_STATUS)
    {
        case TW_START:
            return Status_OK;

        case TW_REP_START:
        default:
            TWCR = 0;
            return Status_Not_OK;
    }
}

Std_ReturnType I2C_Master_RestartCondition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!I2C_Master_TransmitReady());

    switch(TW_STATUS)
    {
        case TW_REP_START:
            return Status_OK;

        case TW_START:
        default:
            TWCR = 0;
            return Status_Not_OK;
    }
}

Std_ReturnType I2C_Master_SlaveWrite(uint8_t slaveAddr)
{
    Std_ReturnType retval = Status_Not_OK;
    uint8_t status = 0;

    retval = I2C_Master_Write( I2C_ADDR(slaveAddr, TW_WRITE), &status );
    if (retval != Status_OK) return retval;

    switch(status)
    {
        case TW_MT_SLA_ACK:
            return Status_OK;

        case TW_MT_SLA_NACK:
        default:
            TWCR = 0;
            return Status_Not_OK;
    }
}

Std_ReturnType I2C_Master_SlaveRead(uint8_t slaveAddr)
{
    Std_ReturnType retval = Status_Not_OK;
    uint8_t status = 0;

    retval = I2C_Master_Write( I2C_ADDR(slaveAddr, TW_READ), &status );
    if (retval != Status_OK) return retval;

    switch(status)
    {
        case TW_MR_SLA_ACK:
            return Status_OK;

        case TW_MR_SLA_NACK:
        default:
            TWCR = 0;
            return Status_Not_OK;
    }
}

Std_ReturnType I2C_Master_StopCondition()
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWSTO);
    while(IS_SET_BIT(TWCR, TWSTO));
    return Status_OK;
}

Std_ReturnType I2C_Master_Write(uint8_t data, uint8_t *status)
{
    TWDR = data;
    TWCR = BIT(TWINT) | BIT(TWEN);
    while (!I2C_Master_TransmitReady());
    if (status != NULL_PTR) *status = TW_STATUS;
    return Status_OK;
}

Std_ReturnType I2C_Master_ReadAck(uint8_t *data)
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWEA);
    while(!I2C_Master_TransmitReady());

    switch(TW_STATUS)
    {
        case TW_MR_DATA_ACK:
            if (data != NULL_PTR) *data = TWDR;
            return Status_OK;

        case TW_MR_DATA_NACK:
        default:
            TWCR = 0;
            return Status_Not_OK;
    }
}

Std_ReturnType I2C_Master_ReadNak(uint8_t *data)
{
    TWCR = BIT(TWINT) | BIT(TWEN);
    while(!I2C_Master_TransmitReady());

    switch(TW_STATUS)
    {
        case TW_MR_DATA_NACK:
            if (data != NULL_PTR) *data = TWDR;
            return Status_OK;

        case TW_MR_DATA_ACK:
        default:
            TWCR = 0;
            return Status_Not_OK;
    }
}
