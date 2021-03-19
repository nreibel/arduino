#ifndef SRC_PCA954X_API_PCA954X_H_
#define SRC_PCA954X_API_PCA954X_H_

#include "types.h"

Std_ReturnType PCA954X_Init();
Std_ReturnType PCA954X_Disable(uint8_t slaveAddr);
Std_ReturnType PCA954X_SelectChannel(uint8_t slaveAddr, uint8_t channel);

#endif /* SRC_PCA954X_API_PCA954X_H_ */
