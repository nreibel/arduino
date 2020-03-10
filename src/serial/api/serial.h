#ifndef SRC_SERIAL_API_SERIAL_H_
#define SRC_SERIAL_API_SERIAL_H_

#include "types.h"
#include "serial_cfg.h"

void Serial_Init();

Std_ReturnType Serial_WriteByte(const uint8_t chr);
Std_ReturnType Serial_WriteBytes(const void * const buffer, const unsigned int length);
Std_ReturnType Serial_PrintString(const char* string);

#if SERIAL_ASYNC_RX == ON
boolean Serial_HasByte();
boolean Serial_HasString();
void Serial_ClearBuffer();
unsigned int Serial_Received();
unsigned int Serial_Read(void *buffer, unsigned int len);
unsigned int Serial_ReadString(void *buffer, unsigned int len);
#else
Std_ReturnType Serial_Read(void * const buffer, const unsigned int buffer_len, unsigned int * const rcvd_len);
#endif

#if SERIAL_ASYNC_TX == ON
Std_ReturnType Serial_TxReady(boolean *ready);
Std_ReturnType Serial_SetTxBuffer(const void * const buffer, const unsigned int length);
#endif

#endif /* SRC_SERIAL_API_SERIAL_H_ */
