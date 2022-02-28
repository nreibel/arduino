#ifndef SERIAL_LL_API_H__
#define SERIAL_LL_API_H__

#include "types.h"
#include "usart.h"

void serial_ll_init(usart_t self, uint32_t baudrate);
void serial_ll_wait_for_tx_ready(usart_t self);
void serial_ll_wait_for_rx_ready(usart_t self);
void serial_ll_write_byte(usart_t self, uint8_t byte);
uint8_t serial_ll_read_byte(usart_t self);

#endif // SERIAL_LL_API_H__
