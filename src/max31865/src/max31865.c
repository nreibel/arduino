#include "max31865.h"
#include "max31865_prv.h"
#include "max31865_cfg.h"
#include "spi.h"
#include "bits.h"
#include "types.h"
#include <math.h>

Std_ReturnType MAX31865_Init(Spi_Slave slave, MAX31865_WireMode mode, MAX31865_FilterMode filter)
{
    Std_ReturnType retval = Status_Not_OK;

    // VBIAS = On, Conversion mode = Auto
    uint8_t configuration = BIT(7) | BIT(6);
    uint8_t read = 0;

    if (mode == MAX31865_WireMode_3_Wires)  SET_BIT(configuration, 4);
    if (filter == MAX31865_FilterMode_50Hz) SET_BIT(configuration, 0);

    // Write config
    Spi_EnableSlave(slave);
    Spi_WriteByte(MAX31865_REG_CONFIG | MAX31865_WRITE, NULL_PTR);
    Spi_WriteByte(configuration, NULL_PTR);
    Spi_DisableSlave(slave);

    // Read back config
    Spi_EnableSlave(slave);
    Spi_WriteByte(MAX31865_REG_CONFIG | MAX31865_READ, NULL_PTR);
    Spi_WriteByte(configuration, &read);
    Spi_DisableSlave(slave);

    // Same config should be read back if communication is set up properly
    if (read == configuration)
    {
        retval = Status_OK;
    }

    return retval;
}

// Implementation of Callendar-Van Dusen equation
// Only for temperatures > 0°C
double MAX31865_RTD_To_Temperature(double rtd)
{
    double acc = 0;
    acc = rtd/MAX31865_RTD_RES_0;
    acc = 1 - acc;
    acc = 4 * MAX31865_RTD_B * acc;
    acc = (MAX31865_RTD_A * MAX31865_RTD_A) - acc;
    acc = sqrt(acc) - MAX31865_RTD_A;
    return acc / (2 * MAX31865_RTD_B);
}

Std_ReturnType MAX31865_ReadRTD(Spi_Slave slave, double *rtd)
{
    Std_ReturnType retval = Status_Not_OK;

    uint8_t msb = 0;
    uint8_t lsb = 0;

    Spi_EnableSlave(slave);
    Spi_WriteByte(MAX31865_REG_RTD_MSB | MAX31865_READ, NULL_PTR);
    Spi_WriteByte(0, &msb);
    Spi_WriteByte(0, &lsb);
    Spi_DisableSlave(slave);

    if ( lsb == 0 && msb == 0 )
    {
        // TODO : device disconnected
    }
    else if ( IS_SET_BIT(lsb, 0) )
    {
        // TODO : handle fault bit
    }
    else
    {
        uint16_t w = (msb << 8) | lsb;
        *rtd = (MAX31865_RES_REF * TYPECAST(w >> 1, double)) / MAX31865_ADC_RESOLUTION;
        retval = Status_OK;
    }

    return retval;
}

// double MAX31865_
