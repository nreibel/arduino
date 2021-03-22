#ifndef __SERIAL_API_H__
#define __SERIAL_API_H__

#include "types.h"
#include "serial_cfg.h"

typedef enum {
    SERIAL_BUS_0,
    NUMBER_OF_SERIAL_BUSES
} serial_bus_t;

#if SERIAL_ASYNC_RX != OFF
typedef void (*serial_rx_callback)(serial_bus_t bus, const char *buffer, int length);
void serial_set_rx_callback(serial_bus_t bus, serial_rx_callback callback);
#endif

#if SERIAL_ASYNC_TX != OFF
bool serial_tx_ready(serial_bus_t bus);
void serial_write_bytes_async(serial_bus_t bus, void *buffer, int len);
#endif

void serial_bus_init(serial_bus_t bus, int baudrate);

int serial_read_byte(serial_bus_t bus, uint8_t *chr);
int serial_read_bytes(serial_bus_t bus, void *buffer, int length);

int serial_write_byte(serial_bus_t bus, uint8_t chr);
int serial_write_bytes(serial_bus_t bus, void *buffer, int length);

int serial_print(serial_bus_t bus, const void* string);
int serial_println(serial_bus_t bus, const void* string);
int serial_new_line(serial_bus_t bus);

// TODO
// int serial_print_P(const __flash void* string);
// int serial_println_P(const __flash void* string);

#endif /* __SERIAL_API_H__ */
