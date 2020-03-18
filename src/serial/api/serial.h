#ifndef SRC_SERIAL_API_SERIAL_H_
#define SRC_SERIAL_API_SERIAL_H_

#include "types.h"
#include "serial_cfg.h"

void Serial_Init();

Std_ReturnType Serial_WriteByte(uint8_t chr);
Std_ReturnType Serial_WriteBytes(void *buffer, int length);
Std_ReturnType Serial_Print(void *string);
Std_ReturnType Serial_Println(void *string);

#if SERIAL_ASYNC_RX == ON
bool Serial_HasByte();
bool Serial_HasString();
void Serial_ClearBuffer();
int Serial_Received();
int Serial_Read(void *buffer, int len);
int Serial_ReadString(void *buffer, int len);
#else
int Serial_Read(void *buffer, int buffer_len);
#endif

#if SERIAL_ASYNC_TX == ON
Std_ReturnType Serial_TxReady(bool *ready);
Std_ReturnType Serial_SetTxBuffer(void * buffer, int length);
#endif

#endif /* SRC_SERIAL_API_SERIAL_H_ */
