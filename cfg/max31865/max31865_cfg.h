#ifndef __MAX31865_CFG_H__
#define __MAX31865_CFG_H__

#define MAX31865_RTD_A_ITS90         3.9080e-3
#define MAX31865_RTD_B_ITS90         -5.870e-7

#define MAX31865_RTD_A_USINDUSTRIAL  3.9692e-3
#define MAX31865_RTD_B_USINDUSTRIAL  -5.8495e-7

#define MAX31865_RTD_A_DIN43760      3.9848e-3
#define MAX31865_RTD_B_DIN43760      -5.8019e-7

// A and B coeffs
#define MAX31865_RTD_A MAX31865_RTD_A_DIN43760
#define MAX31865_RTD_B MAX31865_RTD_B_DIN43760

// RTD resistance in Ohms at 0°C (100 for PT100, 1000 for PT1000)
#define MAX31865_RTD_RES_0 100

// Reference resistor in Ohms
#define MAX31865_RES_REF 430

#endif // __MAX31865_CFG_H__
