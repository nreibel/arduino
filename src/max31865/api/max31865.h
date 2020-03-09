#ifndef SRC_MAX31865_API_MAX31865_H_
#define SRC_MAX31865_API_MAX31865_H_

#include "st7735_cfg.h"

void MAX31865_Init();
Std_ReturnType MAX31865_ReadRTD();
double MAX31865_RTD_To_Temperature(double rtd);

#endif // SRC_MAX31865_API_MAX31865_H_
