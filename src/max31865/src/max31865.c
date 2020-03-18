#include "max31865.h"
#include "max31865_prv.h"
#include "max31865_cfg.h"
#include "spi.h"
#include "bits.h"
#include "types.h"

#include <math.h>

#include <stdio.h>
#include "serial.h"

void MAX31865_Init()
{
    // VBIAS = On, Conversion mode = Auto, 3-wire RTD
    byte configuration = BIT(7) | BIT(6) | BIT(4) | BIT(1) | BIT(0);

    Spi_WriteByte(MAX31865_REG_CONFIG | MAX31865_WRITE, NULL_PTR);
    Spi_WriteByte(configuration, NULL_PTR);
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

Std_ReturnType MAX31865_ReadRTD(double *rtd)
{
    Std_ReturnType retval = Status_Not_OK;

    byte msb = 0;
    byte lsb = 0;

    Spi_WriteByte(MAX31865_REG_RTD_MSB | MAX31865_READ, NULL_PTR);
    Spi_WriteByte(0, &msb);
    Spi_WriteByte(0, &lsb);

    if (IS_SET_BIT(lsb, 0))
    {
        // TODO : handle fault bit
    }
    else
    {
        word w = ((msb << 8) | lsb);
        *rtd = (MAX31865_RES_REF * TYPECAST(w, double)) / MAX31865_ADC_RESOLUTION;
        retval = Status_OK;
    }

    return retval;
}

// double MAX31865_
