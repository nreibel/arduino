#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"

int os_putc(char chr, FILE *stream)
{
    UNUSED(stream);
    serial_write_byte(SERIAL_BUS_0, chr);
    return chr;
}

// App entry point
void app_init()
{
    serial_bus_init(SERIAL_BUS_0, 19200);

    // Init tasks
    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);

    printf("ready!\r\n");
}

// Main task
int task_main(void* data)
{
    UNUSED(data);

    static int cpt = 0;

    if (cpt++ & 1) printf("tock\r\n");
    else printf("tick\r\n");

    return 0;
}
