#ifndef SRC_MAX31855_API_MAX31855_H_
#define SRC_MAX31855_API_MAX31855_H_

#include "spi.h"

typedef struct {
    bool   fault_scv;    // Thermocouple is shorted to Vcc
    bool   fault_scg;    // Thermocouple is shorted to Gnd
    bool   fault_oc;     // Thermocouple is open (no connections)
} MAX31855_Status_t;

Std_ReturnType MAX31855_Init(Spi_Slave slave);
Std_ReturnType MAX31855_GetStatus(Spi_Slave slave, MAX31855_Status_t* status);
Std_ReturnType MAX31855_GetTemperature(Spi_Slave slave, double* temperature, MAX31855_Status_t* status);
Std_ReturnType MAX31855_GetInternalTemperature(Spi_Slave slave, double *temperature);

#endif // SRC_MAX31855_API_MAX31855_H_
