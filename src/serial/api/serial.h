#ifndef SRC_SERIAL_API_SERIAL_H_
#define SRC_SERIAL_API_SERIAL_H_

#include "types.h"
#include "serial_cfg.h"

typedef struct {
    int baudrate;
} serial_t;

void serial_init(serial_t *self, int baudrate);
void serial_write_byte(serial_t *self, uint8_t chr);
void serial_write_bytes(serial_t *self, void *buffer, int length);
void serial_print(serial_t *self, const void* string);
void serial_println(serial_t *self, const void* string);
void serial_new_line(serial_t *self);

// Std_ReturnType Serial_Print_P(const __flash void* string);
// Std_ReturnType Serial_Println_P(const __flash void* string);

#if SERIAL_ASYNC_RX != ON
int serial_read(void *buffer, int buffer_len);
#else
extern void serial_rx_callback(const char *buffer, int length);
#endif

#if SERIAL_ASYNC_TX == ON
void serial_tx_ready(bool *ready);
void serial_set_tx_buffer(void * buffer, int length);
#endif

#endif /* SRC_SERIAL_API_SERIAL_H_ */
