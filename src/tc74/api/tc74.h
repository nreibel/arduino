#ifndef SRC_TC74_API_TC74_H_
#define SRC_TC74_API_TC74_H_

#include "types.h"

Std_ReturnType TC74_Init();
Std_ReturnType TC74_SetStandby(uint8_t slave, bool stdby);
Std_ReturnType TC74_Read(uint8_t slave, uint8_t *temp);

#endif /* SRC_TC74_API_TC74_H_ */
