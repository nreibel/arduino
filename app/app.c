#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"

int os_putc(char chr, FILE *stream)
{
    UNUSED(stream);
    serial_write_byte(SERIAL_BUS[0], chr);
    return chr;
}

// App entry point
void app_init()
{
    serial_bus_init(SERIAL_BUS[0], 19200);

    // Init tasks
    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);

    printf("ready!\r\n");
}

void serial_rx_callback(serial_bus_t bus, const char *buffer, unsigned int length)
{
    UNUSED(bus);
    printf("got size=%u : %s\r\n", length, buffer);
}

// Main task
int task_main(void* data)
{
    UNUSED(data);

    static int cpt = 0;

    if (cpt++ & 1) serial_write_async(SERIAL_BUS[0], "tock\r\n", 6);
    else serial_write_async(SERIAL_BUS[0], "tick\r\n", 6);

    return 0;
}
