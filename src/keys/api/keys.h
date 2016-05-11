#ifndef SRC_KEYS_API_KEYS_H_
#define SRC_KEYS_API_KEYS_H_

#include "types.h"
#include "keys_cfg.h"

void Keys_Init();
void Keys_CyclicTask();

boolean Keys_IsKeyPressed();
Std_ReturnType Keys_GetKeyPressed(Key *key);

#endif /* SRC_KEYS_API_KEYS_H_ */
