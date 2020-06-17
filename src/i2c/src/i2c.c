#include <avr/io.h>
#include "i2c.h"
#include "i2c_cfg.h"
#include "i2c_prv.h"
#include "os.h"
#include "bits.h"
#include "types.h"

Std_ReturnType I2C_Init()
{
    // Enable peripheral
    RESET_BIT(PRR, PRTWI);

#if I2C_MODE == I2C_MODE_MASTER
    TWSR = 0;  // Prescaler = 1
    TWBR = ((F_CPU/F_I2C)-16)/2;
#elif I2C_MODE == I2C_MODE_SLAVE
    TWAR = I2C_SLAVE_ADRESS << 1;
    TWCR = BIT(TWIE) | BIT(TWEA) | BIT(TWINT) | BIT(TWEN);
#else
    #error I2C_MODE not set or incorrect
#endif

    return Status_OK;
}

#if I2C_MODE == I2C_MODE_MASTER

Std_ReturnType I2C_WriteSync(uint8_t addr, void *buffer, int len)
{
    I2C_HAL_StartCondition();
    I2C_HAL_SlaveWrite(addr);
    for (int i = 0 ; i < len ; i++ )
    {
        I2C_HAL_Write( UINT8_PTR(buffer)[i] );
    }
    I2C_HAL_StopCondition();

    return Status_OK;
}

Std_ReturnType I2C_ReadSync(uint8_t addr, void *buffer, int writeLen, int readLen, int delay)
{
    I2C_HAL_StartCondition();
    I2C_HAL_SlaveWrite(addr);
    for(int i = 0 ; i < writeLen ; i++)
    {
        I2C_HAL_Write(UINT8_PTR(buffer)[i]);
    }

    if (delay > 0)
    {
        I2C_HAL_StopCondition();
        Os_Wait(delay);
        I2C_HAL_StartCondition();
    }
    else
    {
        I2C_HAL_RestartCondition();
    }

    I2C_HAL_SlaveRead(addr);
    for(int i = 0 ; i < readLen-1 ; i++)
    {
        UINT8_PTR(buffer)[i] = I2C_HAL_ReadAck();
    }
    UINT8_PTR(buffer)[readLen-1] = I2C_HAL_ReadNak();
    I2C_HAL_StopCondition();

    return Status_OK;
}

#endif

