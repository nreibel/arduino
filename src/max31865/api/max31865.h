#ifndef SRC_MAX31865_API_MAX31865_H_
#define SRC_MAX31865_API_MAX31865_H_

#include "spi.h"

#define MAX31865_RTD_A_ITS90         3.9080e-3
#define MAX31865_RTD_A_USINDUSTRIAL  3.9692e-3
#define MAX31865_RTD_A_DIN43760      3.9848e-3

#define MAX31865_RTD_B_ITS90         -5.870e-7
#define MAX31865_RTD_B_USINDUSTRIAL  -5.8495e-7
#define MAX31865_RTD_B_DIN43760      -5.8019e-7

typedef enum {
    MAX31865_WireMode_2_Wires,
    MAX31865_WireMode_3_Wires,
    MAX31865_WireMode_4_Wires,
} MAX31865_WireMode;

typedef enum {
    MAX31865_FilterMode_50Hz,
    MAX31865_FilterMode_60Hz,
} MAX31865_FilterMode;

Std_ReturnType MAX31865_Init(Spi_Slave slave, MAX31865_WireMode mode, MAX31865_FilterMode filter);
Std_ReturnType MAX31865_ReadRTD(Spi_Slave slave, double *rtd);
double MAX31865_RTD_To_Temperature(double rtd);

#endif // SRC_MAX31865_API_MAX31865_H_
