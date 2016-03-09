#ifndef SRC_SERIAL_API_SERIAL_H_
#define SRC_SERIAL_API_SERIAL_H_

#include "types.h"

void Serial_BackgroundTask();

void Serial_Init();
Std_ReturnType Serial_IsReady();
Std_ReturnType Serial_Print( const char *buffer );

#endif /* SRC_SERIAL_API_SERIAL_H_ */
