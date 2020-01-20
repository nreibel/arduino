#include "spi.h"
#include "spi_prv.h"
#include "spi_cfg.h"
#include "port.h"
#include "bits.h"

static const Spi_Slave* currentSelected = NULL_PTR;
static Spi_TransmitState spiState = Spi_Uninitialized;
static const void * txBuffer = NULL_PTR;
static int txLength = 0;

void Spi_Init()
{
    // Enable MOSI, SCK
    Port_SetPinDataDirection(Spi_MOSI, Output);
    Port_SetPinDataDirection(Spi_SCK, Output);

    // Disable slaves (active high)
    for (uint8_t i = 0 ; i < NbrOfSpiSlaves ; i++)
    {
        Port_SetPinDataDirection(SlaveSelect_Pins[i], Output);
        Port_SetPinState(SlaveSelect_Pins[i], High);
    }

    Spi_Init_HW();

    spiState = Spi_Ready;
}

void Spi_SelectSlave(Spi_Slave slave)
{
    // If already enabled, do nothing
    if (currentSelected != &slave)
    {
        if (currentSelected != NULL_PTR)
        {
            // Disable previously selected slave
            Spi_DisableSlave(*currentSelected);
        }

        // Select slave (active low)
        Port_SetPinState(SlaveSelect_Pins[slave], Low);
        currentSelected = &slave;
    }
}

void Spi_DisableSlave(Spi_Slave slave)
{
    // Disable slave (active low)
    Port_SetPinState(SlaveSelect_Pins[slave], High);
    currentSelected = NULL_PTR;
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

Std_ReturnType Spi_WriteBufferSync(const void * data, int length)
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

Std_ReturnType Spi_WriteBufferAsync(const void *data, int length)
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

boolean Spi_IsReady()
{
    boolean status = FALSE;

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
