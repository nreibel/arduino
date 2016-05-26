#ifndef SRC_SERIAL_API_SERIAL_H_
#define SRC_SERIAL_API_SERIAL_H_

#include "types.h"

void Serial_BackgroundTask();

void Serial_Init();
boolean Serial_IsReady();
Std_ReturnType Serial_AsyncPrint( const char *buffer, int length );

#endif /* SRC_SERIAL_API_SERIAL_H_ */
