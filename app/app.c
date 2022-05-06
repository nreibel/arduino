#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "i2c_ll.h"

#if defined __AVR_ATmega32U4__ // Leonardo
static const usart_t usart = USART1;
#elif defined __AVR_ATmega328P__ // Uno
static const usart_t usart = USART0;
#else
#error "Unknown architecture"
#endif

int os_putc(char chr, FILE *stream)
{
    UNUSED(stream);
    serial_write_byte(usart, chr);
    return chr;
}

void i2c_ll_cbk_rx(uint8_t *buffer, unsigned int length)
{
    printf("received %u bytes on I2C:", length);

    for (unsigned int i = 0 ; i < length ; i++)
        printf(" 0x%02x", buffer[i]);

    printf("\r\n");
}

uint8_t i2c_ll_cbk_tx(unsigned int offset)
{
    uint8_t tx = offset | 0xA0;
    printf("write 0x%02X\r\n", tx);
    return tx;
}

void serial_rx_callback(usart_t usart, volatile const char *buffer, unsigned int length)
{
    UNUSED(usart);
    printf("received %u bytes on USART: %s\r\n", length, buffer);
}

// App entry point
void app_init()
{
    serial_init(usart, 19200);

    i2c_ll_init_slave(TWI0, 0x20);

    // Init tasks
    printf( C_RED "Start!" C_END "\r\n");

    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);
}

// Main task
int task_main(void * data)
{
    UNUSED(data);

    static int cpt = 0;
    if (cpt++ & 1) printf( C_GREY "Tock" C_END "\r\n" );
    else printf( C_GREY "Tick" C_END "\r\n" );

    return EOK;
}
