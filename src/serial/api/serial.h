#ifndef SRC_SERIAL_API_SERIAL_H_
#define SRC_SERIAL_API_SERIAL_H_

#include "types.h"

Std_ReturnType Serial_BackgroundTask();

void Serial_Init();
boolean Serial_IsReady();
Std_ReturnType Serial_Print( const char *buffer, int length );

#endif /* SRC_SERIAL_API_SERIAL_H_ */
