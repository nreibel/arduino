#include <avr/io.h>
#include <util/twi.h>
#include "i2c.h"
#include "i2c_prv.h"
#include "os.h"
#include "bits.h"
#include "types.h"

static void *i2cBufferWrite = NULL_PTR;
static void *i2cBufferRead = NULL_PTR;
static int bufferWriteLen = 0;
static int bufferReadLen = 0;
static uint8_t slaveAddr = 0;

typedef enum {
    I2C_State_Uninitialized = 0,
    I2C_State_Ready,
    I2C_State_Start,
    I2C_State_RepStart,
    I2C_State_Slave_Write,
    I2C_State_Slave_Read,
    I2C_State_Stop,
    I2C_State_Read,
    I2C_State_Write,
} I2C_State;

static I2C_State state = I2C_State_Ready;

Std_ReturnType I2C_BackgroundTask()
{
    Std_ReturnType retval = Status_OK;
    
    switch(state)
    {
        case I2C_State_Uninitialized:
            break;
        case I2C_State_Ready:
            break;
        case I2C_State_Start:
            I2C_StartCondition();
            if (bufferWriteLen > 0) state = I2C_State_Slave_Write;
            else if (bufferReadLen > 0) state = I2C_State_Slave_Read;
            retval = Status_Pending;
            break;
        case I2C_State_RepStart:
            I2C_RestartCondition();
            state = I2C_State_Slave_Read;
            retval = Status_Pending;
            break;
        case I2C_State_Stop:
            I2C_StopCondition();
            if (bufferReadLen > 0) state = I2C_State_Start;
            else state = I2C_State_Ready;
            retval = Status_OK;
            break;
        case I2C_State_Slave_Write:
            I2C_SlaveWrite(slaveAddr);
            state = I2C_State_Write;
            retval = Status_Pending;
            break;
        case I2C_State_Slave_Read:
            I2C_SlaveRead(slaveAddr);
            state = I2C_State_Read;
            retval = Status_Pending;
            break;
        case I2C_State_Read:
            if (bufferReadLen > 1)
            {
                WRITE_PU8(i2cBufferRead++, I2C_ReadAck());
            }
            else
            {
                WRITE_PU8(i2cBufferRead++, I2C_ReadNak());
                state = I2C_State_Stop;
            }
            bufferReadLen--;
            retval = Status_Pending;
            break;
        case I2C_State_Write:
            I2C_Write( READ_PU8(i2cBufferWrite++) );
            bufferWriteLen--;
            if (bufferWriteLen == 0) state = I2C_State_Stop;
            retval = Status_Pending;
            break;
        default:
            HALT;
            break;
    }
    
    return retval;
}

boolean I2C_IsReady()
{
    return state == I2C_State_Ready;
}

void I2C_SetupTransmission(uint8_t addr, void *buffer)
{
    slaveAddr = addr;
    i2cBufferWrite = buffer;
    i2cBufferRead = buffer;
}

void I2C_StartWrite(int writeLen)
{
    bufferWriteLen = writeLen;
    bufferReadLen = 0;
    state = I2C_State_Start;
}

void I2C_StartRead(int writeLen, int readLen)
{
    bufferWriteLen = writeLen;
    bufferReadLen = readLen;
    state = I2C_State_Start;
}

Std_ReturnType I2C_WriteSync(uint8_t addr, void *buffer, int len)
{
    Std_ReturnType retval = Status_Not_OK;
    
    if (state == I2C_State_Ready)
    {
        I2C_StartCondition();
        I2C_SlaveWrite(addr);
        for (int i = 0 ; i < len ; i++ )
        {
            I2C_Write( UINT8_PTR(buffer)[i] );
        }
        I2C_StopCondition();
        retval = Status_OK;
    }
    
    return retval;
}

Std_ReturnType I2C_ReadSync(uint8_t addr, void *buffer, int writeLen, int readLen, int delay)
{
    Std_ReturnType retval = Status_Not_OK;
    
    if (state == I2C_State_Ready)
    {
        I2C_StartCondition();
        I2C_SlaveWrite(addr);
        for(int i = 0 ; i < writeLen ; i++)
        {
            I2C_Write(UINT8_PTR(buffer)[i]);
        }
        
        if (delay > 0)
        {
            I2C_StopCondition();
            Os_Wait(delay);
            I2C_StartCondition();
        }
        else
        {
            I2C_RestartCondition();
        }
        
        I2C_SlaveRead(addr);
        for(int i = 0 ; i < readLen-1 ; i++)
        {
            UINT8_PTR(buffer)[i] = I2C_ReadAck();
        }
        UINT8_PTR(buffer)[readLen-1] = I2C_ReadNak();
        I2C_StopCondition();
        retval = Status_OK;
    }
    
    return retval;
}

void I2C_Init()
{   
    // Enable device
    RESET_BIT(PRR, PRTWI);
    TWSR = 0;  // Prescaler = 1
    TWBR = ((F_CPU/F_I2C)-16)/2;
    state = I2C_State_Ready;
}

boolean I2C_TransmitReady()
{
    return IS_SET_BIT(TWCR, TWINT);
}

void I2C_StartCondition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!I2C_TransmitReady());

    uint8_t status = MASK(TWSR, 0xF8);
    switch(status)
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

void I2C_RestartCondition()
{
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    while (!I2C_TransmitReady());

    uint8_t status = MASK(TWSR, 0xF8);
    switch(status)
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

void I2C_SlaveWrite(uint8_t slaveAddr)
{
    uint8_t status = I2C_Write(I2C_ADDR(slaveAddr) | TW_WRITE);
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

void I2C_SlaveRead(uint8_t slaveAddr)
{
    uint8_t status = I2C_Write(I2C_ADDR(slaveAddr) | TW_READ);
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

void I2C_StopCondition()
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWSTO);
    while(IS_SET_BIT(TWCR, TWSTO));
}

uint8_t I2C_Write(uint8_t data)
{
    TWDR = data;
    TWCR = BIT(TWINT) | BIT(TWEN);
    while (!I2C_TransmitReady());
    return MASK(TWSR, 0xF8);
}

uint8_t I2C_ReadAck(void)
{
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWEA);
    while(!I2C_TransmitReady());
    
    uint8_t status = MASK(TWSR, 0xF8);
    switch(status)
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

uint8_t I2C_ReadNak(void)
{
    TWCR = BIT(TWINT) | BIT(TWEN);
    while(!I2C_TransmitReady());
    
    uint8_t status = MASK(TWSR, 0xF8);
    switch(status)
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
