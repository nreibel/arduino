#ifndef SRC_HC595_API_H_
#define SRC_HC595_API_H_

#include "types.h"

void HC595_Init();
void HC595_Latch();

void HC595_Write(void* data, int len);
void HC595_WriteByte(uint8_t val);
void HC595_WriteWord(uint16_t val);
void HC595_WriteDWord(uint32_t val);

#endif /* SRC_HC595_API_H_ */
