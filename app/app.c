#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "gpio.h"
#include "bits.h"
#include "max31855.h"
#include "st7735.h"
#include "tc74.h"
#include "i2c_master.h"
#include "cat.h"

typedef enum {
    TC74A0,
    TC74A1,
    TC74A2,
    TC74A3,
    TC74A4,
    TC74A5,
    TC74A6,
    TC74A7,
    NUMBER_OF_SENSORS
} sensors_t;

gpio_t st7735_bl, st7735_cs, st7735_dc, max31855_cs;
max31855_t max31855;
st7735_t st7735;
tc74_t tc74[NUMBER_OF_SENSORS];

struct render_circle_data {
    st7735_color_t foreground;
    st7735_color_t background;
};

void serial_received(serial_bus_t bus, const char *buffer, int length)
{
    UNUSED(bus);
    st7735_draw_chars(&st7735, 2, 140, buffer, length, ST7735_COLOR_GREEN, 1);
}

// App entry point
void App_Init()
{
    // Backlight OFF
    gpio_set_data_direction(&st7735_bl, GPIO_OUTPUT);
    gpio_set_state(&st7735_bl, FALSE);

    I2C_Master_Init();
    spi_init();

    gpio_init(&max31855_cs, GPIO_PORT_D, 3);
    gpio_init(&st7735_cs, GPIO_PORT_D, 4);
    gpio_init(&st7735_dc, GPIO_PORT_D, 5);
    gpio_init(&st7735_bl, GPIO_PORT_D, 6);

    max31855_device_init(&max31855, &max31855_cs);

    st7735_init_device(&st7735, &st7735_cs, &st7735_dc, 128, 160);
    st7735_set_offset(&st7735, 2, 1);
    st7735_set_background_color(&st7735, ST7735_COLOR_BLACK);
    st7735_set_orientation(&st7735, ST7735_ORIENTATION_PORTRAIT_INV);
    st7735_clear_screen(&st7735);

    // Backlignt ON
    gpio_set_data_direction(&st7735_bl, GPIO_OUTPUT);
    gpio_set_state(&st7735_bl, TRUE);

    // Init TC74 sensors
    for (int i = 0 ; i < NUMBER_OF_SENSORS ; i++)
    {
        tc74_init(&tc74[i], 0x48+i);
    }

    serial_bus_init(SERIAL_BUS_0, 19200);
    serial_set_rx_callback(SERIAL_BUS_0, serial_received);
    serial_println(SERIAL_BUS_0, "READY");

    Os_SetupTask(Timer_DrawCat, 1000, Task_DrawCat, NULL_PTR);
    Os_SetupTask(Timer_ReadTemp, 200, Task_ReadTemperature, NULL_PTR);
}

// Main task
Std_ReturnType Task_ReadTemperature(void* data)
{
    UNUSED(data);

    static char buffer[32];

    static int temperature[NUMBER_OF_SENSORS];
    static float temperature_avg[NUMBER_OF_SENSORS];

    for (int i = 0 ; i < NUMBER_OF_SENSORS ; i++)
    {
        if (tc74_read_temperature(&tc74[i], &temperature[i]) > 0)
        {
            temperature_avg[i] = (temperature[i] + 3*temperature_avg[i])/4.0;
            sprintf(buffer, "TC74A%1d = %.1f    ", i, temperature_avg[i]);
        }
        else
        {
            sprintf(buffer, "TC74A%1d error     ", i);
        }

        st7735_draw_string(&st7735, 2, i*10, buffer, ST7735_COLOR_WHITE, 1);
    }

    return Status_OK;
}

Std_ReturnType Task_DrawCat(void* data)
{
    UNUSED(data);

    static st7735_xbm_t *picture = xbm_cat_1;
    picture = (picture == xbm_cat_1) ? xbm_cat_2 : xbm_cat_1;
    st7735_draw_xbm(&st7735, picture, 2, 100, XBM_CAT_WIDTH, XBM_CAT_HEIGHT, ST7735_COLOR_CYAN, 2);

    return Status_OK;
}
