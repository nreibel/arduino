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

Std_ReturnType I2C_Master_ReadByteSync(uint8_t addr, uint8_t reg, uint8_t *val)
{
    return I2C_Master_ReadSync(addr, reg, val, 1);
}

Std_ReturnType I2C_Master_WriteByteSync(uint8_t addr, uint8_t reg, uint8_t val)
{
    return I2C_Master_WriteSync(addr, reg, &val, 1);
}

Std_ReturnType I2C_Master_TransmitSync(uint8_t addr, buffer_t buffer, int write_len, int read_len)
{
    Std_ReturnType retval = Status_Not_OK;
    uint8_t *ptr = UINT8_PTR(buffer);

    retval = I2C_Master_StartCondition();
    if (retval != Status_OK) return retval;

    retval = I2C_Master_SlaveWrite(addr);
    if (retval != Status_OK) return retval;

    for(int i = 0 ; i < write_len ; i++)
    {
        retval = I2C_Master_Write(ptr[i], NULL_PTR);
        if (retval != Status_OK) return retval;
    }

    if (read_len > 0)
    {
        retval = I2C_Master_RestartCondition();
        if (retval != Status_OK) return retval;

        retval = I2C_Master_SlaveRead(addr);
        if (retval != Status_OK) return retval;

        for(int i = 0 ; i < read_len - 1 ; i++)
        {
            retval = I2C_Master_ReadAck( &ptr[i] );
            if (retval != Status_OK) return retval;
        }

        retval = I2C_Master_ReadNak( &ptr[read_len-1] );
        if (retval != Status_OK) return retval;
    }

    retval = I2C_Master_StopCondition();
    if (retval != Status_OK) return retval;

    return Status_OK;
}

Std_ReturnType I2C_Master_WriteSync(uint8_t addr, uint8_t reg, buffer_t buffer, int len)
{
    Std_ReturnType retval = Status_Not_OK;

    retval = I2C_Master_StartCondition();
    if (retval != Status_OK) return retval;

    retval = I2C_Master_SlaveWrite(addr);
    if (retval != Status_OK) return retval;

    retval = I2C_Master_Write(reg, NULL_PTR);
    if (retval != Status_OK) return retval;

    for (int i = 0 ; i < len ; i++ )
    {
        retval = I2C_Master_Write( UINT8_PTR(buffer)[i], NULL_PTR );
        if (retval != Status_OK) return retval;
    }

    retval = I2C_Master_StopCondition();
    if (retval != Status_OK) return retval;

    return Status_OK;
}

Std_ReturnType I2C_Master_ReadSync(uint8_t addr, uint8_t reg, buffer_t buffer, int len)
{
    Std_ReturnType retval = Status_Not_OK;
    uint8_t *ptr = UINT8_PTR(buffer);

    retval = I2C_Master_StartCondition();
    if (retval != Status_OK) return retval;

    retval = I2C_Master_SlaveWrite(addr);
    if (retval != Status_OK) return retval;

    retval = I2C_Master_Write(reg, NULL_PTR);
    if (retval != Status_OK) return retval;

    retval = I2C_Master_RestartCondition();
    if (retval != Status_OK) return retval;

    retval = I2C_Master_SlaveRead(addr);
    if (retval != Status_OK) return retval;

    for(int i = 0 ; i < len-1 ; i++)
    {
        retval = I2C_Master_ReadAck( &ptr[i] );
        if (retval != Status_OK) return retval;
    }

    retval = I2C_Master_ReadNak( &ptr[len-1] );
    if (retval != Status_OK) return retval;

    retval = I2C_Master_StopCondition();
    if (retval != Status_OK) return retval;

    return Status_OK;
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
            return Status_Not_OK;

        default:
            return Status_Not_OK;
    }
}

Std_ReturnType I2C_Master_RestartCondition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!I2C_Master_TransmitReady());

    switch(TW_STATUS)
    {
        case TW_START:
            return Status_Not_OK;

        case TW_REP_START:
            return Status_OK;

        default:
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
            return Status_Not_OK;

        default:
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
            return Status_Not_OK;

        default:
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
            return Status_Not_OK;

        default:
            return Status_Not_OK;
    }
}

Std_ReturnType I2C_Master_ReadNak(uint8_t *data)
{
    TWCR = BIT(TWINT) | BIT(TWEN);
    while(!I2C_Master_TransmitReady());

    switch(TW_STATUS)
    {
        case TW_MR_DATA_ACK:
            return Status_Not_OK;

        case TW_MR_DATA_NACK:
            if (data != NULL_PTR) *data = TWDR;
            return Status_OK;

        default:
            return Status_Not_OK;
    }
}
