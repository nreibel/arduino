#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "gpio.h"
#include "bits.h"
#include "max31855.h"
#include "st7735.h"
#include "cat.h"

gpio_t st7735_bl, st7735_cs, st7735_dc, max31855_cs;
max31855_t max31855;
st7735_t st7735;

struct render_circle_data {
    st7735_color_t foreground;
    st7735_color_t background;
};

st7735_color_t render_circle(int x, int y, int w, int h, void *data)
{
    UNUSED(h);
    UNUSED(w);
    UNUSED(x);
    UNUSED(y);

    struct render_circle_data *param = (struct render_circle_data*) data;
    if ( x*x+y*y < w*w) return param->foreground;
    else return param->background;
}

void serial_received(serial_bus_t bus, const char *buffer, int length)
{
    UNUSED(bus);
    st7735_draw_chars(&st7735, 2, 140, buffer, length, ST7735_COLOR_GREEN, 1);
}

// App entry point
void App_Init()
{
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
    st7735_fill_rectangle(&st7735, 10, 50, 18, 18, ST7735_COLOR_ORANGE);
    st7735_draw_char(&st7735, 10, 10, 'A', ST7735_COLOR_FUSCHIA, 3);
    st7735_draw_chars(&st7735, 30, 10, "Hello", 5, ST7735_COLOR_FUSCHIA, 2);
    st7735_draw_string(&st7735, 30, 30, "World", ST7735_COLOR_FUSCHIA, 2);
    st7735_draw_pixel(&st7735, 0, 0, ST7735_COLOR_WHITE);
    st7735_draw_pixel(&st7735, 127, 0, ST7735_COLOR_WHITE);
    st7735_draw_pixel(&st7735, 127, 159, ST7735_COLOR_WHITE);
    st7735_draw_pixel(&st7735, 0, 159, ST7735_COLOR_WHITE);

    struct render_circle_data data = {ST7735_COLOR_YELLOW, ST7735_COLOR_FUSCHIA};
    st7735_render(&st7735, 10, 70, 32, 32, render_circle, &data, 1);

    gpio_set_data_direction(&st7735_bl, GPIO_OUTPUT);
    gpio_set_state(&st7735_bl, TRUE);

    serial_bus_init(SERIAL_BUS_0, 19200);
    serial_set_rx_callback(SERIAL_BUS_0, serial_received);
    serial_println(SERIAL_BUS_0, "READY");

    Os_SetupTask(Timer_MainTask, 500, Task_MainCyclic, NULL_PTR);
}

// Main task
Std_ReturnType Task_MainCyclic(void* data)
{
    UNUSED(data);

    static char str_buffer[16];
    static bool first = TRUE;
    static float temperature_avg = 0.0;
    static st7735_xbm_t *picture = xbm_cat_1;

    float temperature = 0.0;
    max31855_get_internal_temperature(&max31855, &temperature);

    if (first)
    {
        temperature_avg = temperature;
        first = FALSE;
    }
    else
    {
        temperature_avg = (temperature + 3*temperature_avg) / 4;
    }

    sprintf(str_buffer, "It is %.01f'C", temperature_avg);
    serial_println(SERIAL_BUS_0, str_buffer);
    st7735_draw_string(&st7735, 2, 130, str_buffer, ST7735_COLOR_WHITE, 1);

    picture = (picture == xbm_cat_1) ? xbm_cat_2 : xbm_cat_1;
    st7735_draw_xbm(&st7735, picture, 50, 60, XBM_CAT_WIDTH, XBM_CAT_HEIGHT, ST7735_COLOR_CYAN, 2);

    return Status_OK;
}
