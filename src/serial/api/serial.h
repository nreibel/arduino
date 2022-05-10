#ifndef __SERIAL_API_H__
#define __SERIAL_API_H__

#include "types.h"
#include "serial_ll.h"
#include "serial_cfg.h"

enum {
    SERIAL_OK,
    SERIAL_ERROR_INSTANCE,
    SERIAL_ERROR_BUSY,
};

#if SERIAL_ASYNC_RX
extern void serial_rx_overflow(usart_t usart);
extern void serial_rx_callback(usart_t usart, volatile const char *buffer, unsigned int length);
#endif

#if SERIAL_ASYNC_TX
bool serial_tx_ready(usart_t usart);
int serial_write_async(usart_t usart, const void * buffer, unsigned int length);
#endif

int serial_init(usart_t usart, uint32_t baudrate);

int serial_read_byte(usart_t usart, uint8_t *chr);
int serial_read_bytes(usart_t usart, void * buffer, unsigned int length);

int serial_write_byte(usart_t usart, uint8_t chr);
int serial_write_bytes(usart_t usart, const void * buffer, unsigned int length);

int serial_print(usart_t usart, const char * string);
int serial_println(usart_t usart, const char * string);
int serial_new_line(usart_t usart);

int serial_print_P(usart_t usart, flstr_t string);
int serial_println_P(usart_t usart, flstr_t string);

#endif /* __SERIAL_API_H__ */
