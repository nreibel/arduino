#include "max31855.h"
#include "types.h"
#include "bits.h"
#include "spi.h"

Std_ReturnType MAX31855_Init(Spi_Slave slave)
{
    UNUSED(slave);

    // Nothing to do

    return Status_OK;
}


Std_ReturnType MAX31855_GetStatus(Spi_Slave slave, MAX31855_Status_t* status)
{
    uint8_t bytes[2] = {0};

    Spi_Configure(SPI_CLOCK_DIV_2, SPI_MODE_1);
    Spi_EnableSlave(slave);
    Spi_ReadBytes(bytes, 2); // External temp
    Spi_ReadBytes(bytes, 2); // Internal temp + error flags
    Spi_DisableSlave(slave);

    status->fault_scv = IS_SET_BIT(bytes[1], 2);
    status->fault_scg = IS_SET_BIT(bytes[1], 1);
    status->fault_oc  = IS_SET_BIT(bytes[1], 0);

    return Status_OK;
}

Std_ReturnType MAX31855_GetInternalTemperature(Spi_Slave slave, double *temperature)
{
    uint8_t bytes[2] = {0};

    Spi_Configure(SPI_CLOCK_DIV_2, SPI_MODE_1);
    Spi_EnableSlave(slave);
    Spi_ReadBytes(bytes, 2); // External temp
    Spi_ReadBytes(bytes, 2); // Internal temp + error flags
    Spi_DisableSlave(slave);

    double ipart = bytes[0];
    double fpart = HIGH_NIBBLE(bytes[1]) / 16.0;
    *temperature = ipart + fpart;

    // Two's complement
    if (bytes[0] >= 0x80) *temperature -= 256;

    return Status_OK;
}

Std_ReturnType MAX31855_GetTemperature(Spi_Slave slave, double *temp, MAX31855_Status_t* status)
{
    Std_ReturnType retval = Status_Not_OK;
    uint8_t bytes[2] = {0};

    // Read first 2 bytes (temperature + fault bit)
    Spi_Configure(SPI_CLOCK_DIV_2, SPI_MODE_1);
    Spi_EnableSlave(slave);
    Spi_ReadBytes(bytes, 2);

    if (IS_SET_BIT(bytes[1], 0))
    {
        if (status != NULL_PTR)
        {
            // Next 2 bytes contain error flags
            Spi_ReadBytes(bytes, 2);

            status->fault_scv = IS_SET_BIT(bytes[1], 2);
            status->fault_scg = IS_SET_BIT(bytes[1], 1);
            status->fault_oc  = IS_SET_BIT(bytes[1], 0);
        }
    }
    else
    {
        double ipart = (bytes[0] << 4) + (bytes[1] >> 4);
        double fpart = ((bytes[1] >> 2) & 0x3) / 4.0;
        *temp = ipart + fpart;

        // Two's complement
        if (bytes[0] >= 0x80) *temp -= 4096;

        retval = Status_OK;
    }

    Spi_DisableSlave(slave);

    return retval;
}
