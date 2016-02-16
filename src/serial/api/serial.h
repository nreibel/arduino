#ifndef SRC_SERIAL_API_SERIAL_H_
#define SRC_SERIAL_API_SERIAL_H_

#include "types.h"

Std_ReturnType Serial_IsReady();
Std_ReturnType Serial_Print( const char *buffer );

#endif /* SRC_SERIAL_API_SERIAL_H_ */
