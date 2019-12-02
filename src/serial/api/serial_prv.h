#ifndef SRC_SERIAL_API_SERIAL_PRV_H_
#define SRC_SERIAL_API_SERIAL_PRV_H_

#include "types.h"
#include "serial_cfg.h"

#if SERIAL_ASYNC_RX == ON
void Serial_HAL_EnableRxInterrupts();
void Serial_HAL_ISR_Rx();
#endif

#if SERIAL_ASYNC_TX == ON
void Serial_HAL_EnableTxInterrupts();
void Serial_HAL_ISR_Tx();
#endif

/* Hardware abstraction */
void Serial_HAL_Init_HW(void);
boolean Serial_HAL_TxIsReady(void);
boolean Serial_HAL_RxIsReady(void);
void Serial_HAL_WriteByte(uint8_t byte);
uint8_t Serial_HAL_ReadByte(void);

#endif /* SRC_SERIAL_API_SERIAL_PRV_H_ */
