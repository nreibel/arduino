#ifndef SRC_SERIAL_API_SERIAL_TP_H_
#define SRC_SERIAL_API_SERIAL_TP_H_

#include "types.h"

void Serial_TP_Init();
void Serial_TP_Cyclic();
bool Serial_TP_Ready();
int Serial_TP_Read(void* buffer, int len);

#endif /* SRC_SERIAL_API_SERIAL_TP_H_ */
