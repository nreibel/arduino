#include "serial.h"
#include "serial_cfg.h"
#include "serial_prv.h"
#include "os_cfg.h"
#include "types.h"
#include "bits.h"

#include <avr/pgmspace.h>

void serial_bus_init(serial_bus_t bus, int baudrate)
{
    serial_hal_init(bus, baudrate);
}

#if SERIAL_ASYNC_RX != OFF
void serial_set_rx_callback(serial_bus_t bus, serial_rx_callback cbk)
{
    serial_hal_set_rx_callback(bus, cbk);
}
#endif

#if SERIAL_ASYNC_TX != OFF
bool serial_tx_ready(serial_bus_t bus)
{
    return serial_hal_tx_buffer_ready(bus);
}
void serial_write_bytes_async(serial_bus_t bus, void *buffer, int length)
{
    serial_hal_set_tx_buffer(bus, buffer, length);
}
#endif

int serial_write_byte(serial_bus_t bus, uint8_t chr)
{
    serial_hal_write_byte(bus, chr);
    while( !serial_hal_tx_ready(bus) );
    return 1;
}

int serial_write_bytes(serial_bus_t bus, void *buffer, int length)
{
    int written = 0;

    for (int i = 0 ; i < length ; i++)
    {
        written += serial_write_byte(bus, READ_PU8(buffer+i) );
    }

    return written;
}

int serial_new_line(serial_bus_t bus)
{
    return serial_write_bytes(bus, "\r\n", 2);
}

int serial_print(serial_bus_t bus, const void* string)
{
    int written = 0;

    for(uint8_t *b = UINT8_PTR(string) ; *b != 0 ; b++)
    {
        written += serial_write_byte(bus, *b);
    }

    return written;
}

int serial_println(serial_bus_t bus, const void* string)
{
    return serial_print(bus, string) + serial_new_line(bus);
}

// void serial_print_P(serial_bus_t bus, const __flash void* string)
// {
//     while (TRUE)
//     {
//         uint8_t b = pgm_read_byte(string++);
//         if (b != 0) serial_write_byte(bus, b);
//         else break;
//     }
// }
//
// void serial_println_P(serial_bus_t bus, const __flash void* string)
// {
//     serial_print_P(bus, string);
//     serial_new_line(bus);
// }

int serial_read_byte(serial_bus_t bus, uint8_t *byte)
{
    while( !serial_hal_rx_ready(bus) );
    *byte = serial_hal_read_byte(bus);
    return 1;
}

int serial_read_bytes(serial_bus_t bus, void *buffer, int length)
{
    int received = 0;

    for(int i = 0 ; i < length; i++)
    {
        received += serial_read_byte(bus, UINT8_PTR(buffer+i) );
    }

    return received;
}
