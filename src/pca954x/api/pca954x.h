#ifndef SRC_PCA954X_API_PCA954X_H_
#define SRC_PCA954X_API_PCA954X_H_

#include "types.h"

Std_ReturnType PCA954X_Init();
Std_ReturnType PCA954X_SetEnabled(uint8_t slaveAddr, bool enabled);
Std_ReturnType PCA954X_SelectChannel(uint8_t slaveAddr, uint8_t channel);

#endif /* SRC_PCA954X_API_PCA954X_H_ */
