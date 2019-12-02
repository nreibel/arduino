#ifndef SRC_SERIAL_API_SERIAL_H_
#define SRC_SERIAL_API_SERIAL_H_

#include "types.h"
#include "serial_cfg.h"

void Serial_Init();

Std_ReturnType Serial_Print(const void * const buffer, const unsigned int length);

#if SERIAL_ASYNC_RX == ON
Std_ReturnType Serial_RxReady(boolean *ready);
Std_ReturnType Serial_GetRxBuffer(void **buffer, unsigned int *rcvd_len);
#else
Std_ReturnType Serial_Read(void * const buffer, const unsigned int buffer_len, unsigned int * const rcvd_len);
#endif

#if SERIAL_ASYNC_TX == ON
Std_ReturnType Serial_TxReady(boolean *ready);
Std_ReturnType Serial_SetTxBuffer(const void * const buffer, const unsigned int length);
#endif

#endif /* SRC_SERIAL_API_SERIAL_H_ */
