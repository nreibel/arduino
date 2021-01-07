#ifndef SRC_SERIAL_API_SERIAL_H_
#define SRC_SERIAL_API_SERIAL_H_

#include "types.h"
#include "serial_cfg.h"

void Serial_Init();

Std_ReturnType Serial_WriteByte(uint8_t chr);
Std_ReturnType Serial_WriteBytes(void *buffer, int length);

Std_ReturnType Serial_NewLine();
Std_ReturnType Serial_Print_P(const __flash void* string);
Std_ReturnType Serial_Println_P(const __flash void* string);
Std_ReturnType Serial_Print(const void* string);
Std_ReturnType Serial_Println(const void* string);

#if SERIAL_ASYNC_RX != ON
int Serial_Read(void *buffer, int buffer_len);
#else
void Serial_RxCallback(const char *buffer, int length);
#endif

#if SERIAL_ASYNC_TX == ON
Std_ReturnType Serial_TxReady(bool *ready);
Std_ReturnType Serial_SetTxBuffer(void * buffer, int length);
#endif

#endif /* SRC_SERIAL_API_SERIAL_H_ */
