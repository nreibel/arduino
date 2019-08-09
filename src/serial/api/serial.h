#ifndef SRC_SERIAL_API_SERIAL_H_
#define SRC_SERIAL_API_SERIAL_H_

#include "types.h"

Std_ReturnType Serial_BackgroundTask();

void Serial_Init();
boolean Serial_IsReady();

Std_ReturnType Serial_Print(const uint8_t *buffer, const int length);
Std_ReturnType Serial_AsyncPrint(const uint8_t *buffer, const int length);

Std_ReturnType Serial_Read(uint8_t *buffer, const int buffer_len, int *rcvd_len);

Std_ReturnType Serial_DataReady(boolean *ready);
Std_ReturnType Serial_AsyncRead(uint8_t *buffer, const int buffer_len, int *rcvd_len);

#endif /* SRC_SERIAL_API_SERIAL_H_ */
