#ifndef SRC_SERIAL_API_SERIAL_PRV_H_
#define SRC_SERIAL_API_SERIAL_PRV_H_

#include "types.h"
#include "serial_cfg.h"

#if SERIAL_ASYNC_RX == ON
void serial_hal_enable_rx_interrupts();
void serial_hal_isr_rx();
#endif

#if SERIAL_ASYNC_TX == ON
void serial_hal_enable_tx_interrupts();
void serial_hal_isr_tx();
#endif

// Hardware abstraction
void serial_hal_init(int baudrate);
bool serial_hal_tx_ready();
bool serial_hal_rx_ready();
void serial_hal_write_byte(uint8_t byte);
uint8_t serial_hal_read_byte();

#endif /* SRC_SERIAL_API_SERIAL_PRV_H_ */
