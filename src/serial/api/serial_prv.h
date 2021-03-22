#ifndef __SERIAL_PRV_H__
#define __SERIAL_PRV_H__

#include "types.h"
#include "serial.h"
#include "serial_cfg.h"

#if SERIAL_ASYNC_RX != OFF
void serial_hal_set_rx_callback(serial_bus_t bus, serial_rx_callback cbk);
#endif

#if SERIAL_ASYNC_TX != OFF
bool serial_hal_tx_buffer_ready(serial_bus_t bus);
void serial_hal_set_tx_buffer(serial_bus_t bus, void *buffer, int length);
#endif

void serial_hal_init(serial_bus_t bus, int baudrate);
bool serial_hal_tx_ready(serial_bus_t bus);
bool serial_hal_rx_ready(serial_bus_t bus);
void serial_hal_write_byte(serial_bus_t bus, uint8_t byte);
uint8_t serial_hal_read_byte(serial_bus_t bus);

#endif /* __SERIAL_PRV_H__ */
