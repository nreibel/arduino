#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial_ll.h"

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
    serial_ll_write(usart, chr);
    serial_ll_wait_tx(usart);
    return chr;
}

// App entry point
void app_init()
{
    serial_ll_init(usart, 19200);

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

    return 0;
}
