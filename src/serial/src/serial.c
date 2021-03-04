#include "serial.h"
// #include "serial_cfg.h"
#include "serial_prv.h"
#include "os_cfg.h"
#include "types.h"
#include "bits.h"

#include <avr/pgmspace.h>

void serial_init(serial_t *self, int baudrate)
{
    self->baudrate = baudrate;

    serial_hal_init(baudrate);

    #if SERIAL_ASYNC_RX == ON
    serial_hal_enable_rx_interrupts();
    #endif

    #if SERIAL_ASYNC_TX == ON
    serial_hal_enable_tx_interrupts();
    #endif
}

void serial_write_byte(serial_t *self, uint8_t chr)
{
    UNUSED(self);

    serial_hal_write_byte(chr);
    while( !serial_hal_tx_ready() );
}

void serial_write_bytes(serial_t *self, void *buffer, int length)
{
    while ( length-- > 0 )
    {
        uint8_t b = READ_PU8(buffer++);
        serial_write_byte(self, b);
    }
}

void serial_new_line(serial_t *self)
{
    return serial_write_bytes(self, "\r\n", 2);
}

void serial_println(serial_t *self, const void* string)
{
    serial_print(self, string);
    serial_new_line(self);
}

void serial_print_P(serial_t *self, const __flash void* string)
{
    while (TRUE)
    {
        uint8_t b = pgm_read_byte(string++);
        if (b != 0) serial_write_byte(self, b);
        else break;
    }
}

void serial_println_P(serial_t *self, const __flash void* string)
{
    serial_print_P(self, string);
    serial_new_line(self);
}

void serial_print(serial_t *self, const void* string)
{
    while (TRUE)
    {
        uint8_t b = READ_PU8(string++);
        if (b != 0) serial_write_byte(self, b);
        else break;
    }
}

#if SERIAL_ASYNC_RX == OFF
int serial_read(void *buffer, int buffer_len)
{
    int rcvd_len = 0;

    // Stop when buffer full
    while ( rcvd_len < buffer_len )
    {
        while( !serial_hal_RxIsReady() ); // Wait for RX complete
        UINT8_PTR(buffer)[rcvd_len++] = serial_hal_ReadByte();
    }

    return rcvd_len;
}
#endif // SERIAL_ASYNC_RX == OFF
