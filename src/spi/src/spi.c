#include "spi.h"
#include "spi_prv.h"
#include "spi_cfg.h"
#include "port.h"
#include "bits.h"
#include <avr/io.h>

static const Spi_Slave* currentSelected = NULL_PTR;
static Spi_TransmitState s_state = Spi_Uninitialized;
static const uint8_t *s_buffer = NULL_PTR;
static int s_length = 0;

void Spi_Init()
{
    // Enable peripheral
    RESET_BIT(PRR, PRSPI);

    // Enable MOSI, SCK
    Port_SetPinDataDirection(Spi_MOSI, Output);
    Port_SetPinDataDirection(Spi_SCK, Output);

    // Disable slaves (active high)
    for (int i = 0 ; i < NbrOfSpiSlaves ; i++)
    {
        Port_SetPinDataDirection(SlaveSelect_Pins[i], Output);
        Port_SetPinState(SlaveSelect_Pins[i], High);
    }

    // Enable SPI Master Mode
    SPCR = BIT(MSTR) | BIT(SPE) | MASK(SPI_PRV_SPCR_SPR, 0xFF);

    s_state = Spi_Ready;
}

void Spi_SelectSlave(const Spi_Slave slave)
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

void Spi_DisableSlave(const Spi_Slave slave)
{
    // Disable slave (active low)
    Port_SetPinState(SlaveSelect_Pins[slave], High);
    currentSelected = NULL_PTR;
}

Std_ReturnType Spi_WriteByte(const uint8_t write, uint8_t *const read)
{
    Std_ReturnType retval = Status_Not_OK;

    if (s_state == Spi_Ready)
    {
        SPDR = write;
        while( ! IS_SET_BIT(SPSR, SPIF) ); // Wait transmission
        if (read != NULL_PTR)
        {
            *read = SPDR;
        }
    }

    return retval;
}

void Spi_FastWrite(const uint8_t write)
{
    SPDR = write;
    asm volatile ("nop"); // At high speed, can delay and prevent entering the loop
    while( ! IS_SET_BIT(SPSR, SPIF) ); // Wait transmission
}

Std_ReturnType Spi_WriteBufferSync(const uint8_t *const data, const int length)
{
    Std_ReturnType retval = Status_Not_OK;

    if (s_state == Spi_Ready && data != NULL_PTR && length > 0)
    {
        for (int i = 0 ; i < length ; i++)
        {
            SPDR = data[i];
            while( ! IS_SET_BIT(SPSR, SPIF) ); // Wait transmission
        }

        retval = Status_OK;
    }

    return retval;
}

Std_ReturnType Spi_WriteBufferAsync(const uint8_t *const data, const int length)
{
    Std_ReturnType ret = Status_Not_OK;

    if (s_state == Spi_Ready && data != NULL_PTR && length > 0)
    {
        s_buffer = data;
        s_length = length;
        s_state = Spi_Writing;
        ret = Status_Pending;
    }

    return ret;
}

boolean Spi_IsReady()
{
    boolean status = FALSE;

    if (s_state == Spi_Ready)
    {
        status = TRUE;
    }

    return status;
}

Std_ReturnType Spi_BackgroundTask()
{
    Std_ReturnType retval = Status_Pending;

    switch(s_state)
    {
    case Spi_Uninitialized:
        retval = Status_Not_OK;
        break;

    case Spi_Ready:
        retval = Status_OK;
        break;

    case Spi_Writing:
        SPDR = *(s_buffer++);
        s_length--;
        s_state = Spi_Waiting;
        break;

    case Spi_Waiting:
        if (IS_SET_BIT(SPSR, SPIF))
        {
            s_state = s_length > 0 ? Spi_Writing : Spi_Ready;
        }
        break;

    default:
        retval = Status_Not_OK;
        break;
    }

    return retval;
}
