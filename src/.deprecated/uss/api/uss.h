#ifndef SRC_USS_API_USS_H_
#define SRC_USS_API_USS_H_

#include "types.h"

void USS_Init();
Std_ReturnType USS_GetDistance(uint16_t *distance);
Std_ReturnType USS_TriggerMeasurement();

#endif /* SRC_USS_API_USS_H_ */
