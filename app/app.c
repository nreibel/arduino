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

char buffer[64];

gpio_t gpio_backlight, gpio_st7735_cs, gpio_st7735_dc, gpio_max31855_cs;
max31855_t max31855;
st7735_t st7735;
tc74_t tc74;

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

st7735_xbm_t *xbm = xbm_cat_1;

// App entry point
void App_Init()
{
    I2C_Master_Init();
    spi_init();

    gpio_init(&gpio_st7735_dc, GPIO_PORT_D, 7);
    gpio_init(&gpio_max31855_cs, GPIO_PORT_B, 1);
    gpio_init(&gpio_st7735_cs, GPIO_PORT_B, 2);
    gpio_init(&gpio_backlight, GPIO_PORT_D, 5);

    tc74_init(&tc74, 0x90);

    max31855_device_init(&max31855, &gpio_max31855_cs);

    st7735_init_device(&st7735, &gpio_st7735_cs, &gpio_st7735_dc, 128, 160);
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

    st7735_draw_xbm(&st7735, xbm, 50, 60, XBM_CAT_WIDTH, XBM_CAT_HEIGHT, ST7735_COLOR_CYAN, 2);

    Serial_Init();
    Serial_Println("READY");

    gpio_set_data_direction(&gpio_backlight, GPIO_OUTPUT);
    gpio_set_state(&gpio_backlight, TRUE);

    Os_SetupTask(Timer_MainTask, 500, Task_MainCyclic, NULL_PTR);
}

// Main task
Std_ReturnType Task_MainCyclic(void* data)
{
    UNUSED(data);

    int temperature_tc74 = 0;
    tc74_read_temperature(&tc74, &temperature_tc74);
    sprintf(buffer, "TC74 Temperature is %d", temperature_tc74);
    Serial_Println(buffer);

    double temperature_int = 0.0;
    max31855_get_internal_temperature(&max31855, &temperature_int);
    sprintf(buffer, "MAX31855 Internal Temperature is %f", temperature_int);
    Serial_Println(buffer);

    static double temperature_int_avg = 0;
    temperature_int_avg = (temperature_int + 3*temperature_int_avg)/4;
    sprintf(buffer, "%6.02f'C", temperature_int_avg);
    st7735_draw_string(&st7735, 30, 50, buffer, ST7735_COLOR_FUSCHIA, 1);

    double temperature_ext = 0.0;
    if ( max31855_get_temperature(&max31855, &temperature_ext) )
    {
        sprintf(buffer, "MAX31855 External Temperature is %f", temperature_ext);
        Serial_Println(buffer);
    }
    else
    {
        Serial_Println("MAX31855 Temperature probe error");
    }

    xbm = (xbm == xbm_cat_1) ? xbm_cat_2 : xbm_cat_1;
    st7735_draw_xbm(&st7735, xbm, 50, 60, XBM_CAT_WIDTH, XBM_CAT_HEIGHT, ST7735_COLOR_CYAN, 2);

    return Status_OK;
}
