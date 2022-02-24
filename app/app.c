#include "stdio.h"
#include "os.h"
#include "app.h"
#include "gpio.h"
#include "serial.h"

static struct gpio_prv_s led_prv_t;
static gpio_t led = &led_prv_t;

int os_putc(char chr, FILE *stream)
{
    UNUSED(stream);
    serial_write_byte(SERIAL_BUS[0], chr);
    return chr;
}

void extint_cbk(extint_t pin, volatile void * data)
{
    UNUSED(data);
    printf("ext int %u\r\n", pin);
}

void pcint_cbk(pcint_t port, uint8_t mask, volatile void * data)
{
    UNUSED(data);
    printf("pcint %u 0x%02X\r\n", port, mask);
}

// App entry point
void app_init()
{
    serial_bus_init(SERIAL_BUS[0], 19200);

    gpio_init(led, PORT_C, 7, GPIO_OUTPUT_ACTIVE_HIGH);

    gpio_enable_extint(EXTINT_0, GPIO_EDGE_RISING, extint_cbk, NULL_PTR);
    gpio_enable_extint(EXTINT_1, GPIO_EDGE_RISING, extint_cbk, NULL_PTR);
    gpio_enable_extint(EXTINT_6, GPIO_EDGE_RISING, extint_cbk, NULL_PTR);
    gpio_enable_pcint(PCINT_B, 0xFF, pcint_cbk, NULL_PTR);

    printf("Start!\r\n");
    os_wait(10);

    // Init tasks
    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);
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

    if (cpt++ & 1)
    {
        gpio_reset(led);
        serial_write_async(SERIAL_BUS[0], "tock\r\n", 6);
    }
    else
    {
        gpio_set(led);
        serial_write_async(SERIAL_BUS[0], "tick\r\n", 6);
    }

    return 0;
}
