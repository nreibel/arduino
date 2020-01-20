#ifndef SRC_HC595_API_H_
#define SRC_HC595_API_H_

#include "types.h"
#include "hc595_cfg.h"

void HC595_Init();
void HC595_Latch();
void HC595_ShiftBit(uint8_t bit);

void HC595_ShiftByte(uint8_t val);

#if HC595_DAISY_CHAIN_COUNT > 1
void HC595_ShiftWord(uint16_t val);
#endif

#if HC595_DAISY_CHAIN_COUNT > 3
void HC595_ShiftDWord(uint32_t val);
#endif

#if HC595_PIN_CLEAR == ON
void HC595_Clear();
#endif

#endif /* SRC_HC595_API_H_ */
