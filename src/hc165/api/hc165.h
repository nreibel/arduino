#ifndef SRC_HC165_API_H_
#define SRC_HC165_API_H_

#include "types.h"

void HC165_Init();

int HC165_Read(void* buffer, int len);
uint8_t HC165_ReadByte();
uint16_t HC165_ReadWord();
uint32_t HC165_ReadDWord();

#endif /* SRC_HC165_API_H_ */
