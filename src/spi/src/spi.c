#include "spi.h"
#include "spi_prv.h"
#include "spi_cfg.h"
#include "gpio.h"
#include "bits.h"

static Spi_TransmitState spiState = Spi_Uninitialized;
static const void * txBuffer = NULL_PTR;
static int txLength = 0;

void Spi_Init()
{
    // Enable MOSI, SCK
    GPIO_SetDataDirection(Spi_MOSI, GPIO_Output);
    GPIO_SetDataDirection(Spi_SCK, GPIO_Output);

    // Disable slaves (active high)
    for (uint8_t i = 0 ; i < SPI_NUMBER_OF_SLAVES ; i++)
    {
        GPIO_SetDataDirection(SlaveSelect_Pins[i], GPIO_Output);
        GPIO_Set(SlaveSelect_Pins[i], GPIO_High);
    }

    Spi_HAL_Enable();

    spiState = Spi_Ready;
}

void Spi_Configure(Spi_Clock clock, Spi_Mode mode)
{
    Spi_HAL_Configure(clock, mode);
}

void Spi_EnableSlave(Spi_Slave slave)
{
    // Enable slave (active low)
    GPIO_Set(SlaveSelect_Pins[slave], GPIO_Low);
}

void Spi_DisableSlave(Spi_Slave slave)
{
    // Disable slave (active low)
    GPIO_Set(SlaveSelect_Pins[slave], GPIO_High);
}

Std_ReturnType Spi_WriteByte(uint8_t write, uint8_t *read)
{
    Std_ReturnType retval = Status_Not_OK;

    if (spiState == Spi_Ready)
    {
        Spi_HAL_WriteByte(write);
        while( !Spi_HAL_IsReady() ); // Wait end of transmission

        if (read != NULL_PTR)
        {
            *read = Spi_HAL_ReadByte();
        }

        retval = Status_OK;
    }

    return retval;
}

Std_ReturnType Spi_WriteBytes(void* data, int length)
{
    Std_ReturnType retval = Status_Not_OK;

    if (spiState == Spi_Ready && data != NULL_PTR)
    {
        while(length-- > 0)
        {
            Spi_HAL_WriteByte( READ_PU8(data++) );
            while( !Spi_HAL_IsReady() ); // Wait end of transmission
        }

        retval = Status_OK;
    }

    return retval;
}

Std_ReturnType Spi_WriteAsync(void* data, int length)
{
    Std_ReturnType ret = Status_Not_OK;

    if (spiState == Spi_Ready && data != NULL_PTR && length > 0)
    {
        txBuffer = data;
        txLength = length;
        spiState = Spi_Writing;
        ret = Status_Pending;
    }

    return ret;
}

bool Spi_IsReady()
{
    bool status = FALSE;

    if (spiState == Spi_Ready)
    {
        status = TRUE;
    }

    return status;
}

Std_ReturnType Spi_BackgroundTask()
{
    Std_ReturnType retval = Status_Pending;

    switch(spiState)
    {
    case Spi_Uninitialized:
        retval = Status_Not_OK;
        break;

    case Spi_Ready:
        retval = Status_OK;
        break;

    case Spi_Writing:
        Spi_HAL_WriteByte( READ_PU8(txBuffer++) );
        txLength--;
        spiState = Spi_Waiting;
        break;

    case Spi_Waiting:
        if (Spi_HAL_IsReady())
        {
            spiState = txLength > 0 ? Spi_Writing : Spi_Ready;
        }
        break;

    default:
        retval = Status_Not_OK;
        break;
    }

    return retval;
}
