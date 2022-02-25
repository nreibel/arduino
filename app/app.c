#include "stdio.h"
#include "os.h"
#include "app.h"
#include "gpio.h"
#include "serial.h"
#include "i2c.h"
#include "i2c_drv.h"
#include "tc74.h"

static gpio_t led = NULL_PTR;
static tc74_t tc74 = NULL_PTR;
static i2c_bus_t i2c = NULL_PTR;
// extern int i2c_driver_init(twi_t bus, bool fast_mode);

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
    printf("pcint %u port=0x%02X\r\n", port, mask);
}

// App entry point
void app_init()
{
    serial_bus_init(SERIAL_BUS[0], 19200);

    i2c = i2c_driver_create(TWI0, FALSE);
    tc74 = tc74_create(i2c, TC74A4);

    led = gpio_create(PORT_B, 5, GPIO_OUTPUT_ACTIVE_HIGH);

//     gpio_enable_extint(EXTINT_0, GPIO_EDGE_RISING, extint_cbk, NULL_PTR);
//     gpio_enable_extint(EXTINT_1, GPIO_EDGE_RISING, extint_cbk, NULL_PTR);
//     gpio_enable_extint(EXTINT_6, GPIO_EDGE_RISING, extint_cbk, NULL_PTR);
//     gpio_enable_pcint(PCINT_B, 0x0F, pcint_cbk, NULL_PTR);

    printf("Start!\r\n");

    // Init tasks
    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);
}

void serial_rx_callback(serial_bus_t bus, const char *buffer, unsigned int length)
{
    UNUSED(bus);
    printf("received %u bytes : [%s]\r\n", length, buffer);
}

// Main task
int task_main(void* data)
{
    UNUSED(data);

    gpio_toggle(led);

    static int cpt = 0;
    if (cpt++ & 1) printf("tock\r\n");
    else printf("tick\r\n");

    int temperature = 0;
    int res = tc74_read_temperature(tc74, &temperature);
    if (res < 0) printf("temperature : read error %d\r\n", res);
    else printf("temperature : %d°C\r\n", temperature);

    return 0;
}
