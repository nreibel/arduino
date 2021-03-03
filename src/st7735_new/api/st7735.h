#ifndef SRC_ST7735_API_ST7735_H_
#define SRC_ST7735_API_ST7735_H_

#include "types.h"
#include "spi.h"
#include "gpio.h"

typedef uint16_t st7735_color_t;
typedef const __flash uint8_t st7735_xbm_t;
// typedef char* st7735_xpm_t;
typedef st7735_color_t (*ST7735_Renderer)(int x, int y, int w, int h, void* data);

typedef struct {
    spi_device_t spi_device;
    st7735_color_t background_color;
    gpio_t *dc;
    int width;
    int height;
    int offset_x;
    int offset_y;
} st7735_t;

typedef enum {
    ST7735_ORIENTATION_PORTRAIT,
    ST7735_ORIENTATION_LANDSCAPE,
    ST7735_ORIENTATION_PORTRAIT_INV,
    ST7735_ORIENTATION_LANDSCAPE_INV
} st7735_orientation_t;

void st7735_init_device(st7735_t *self, gpio_t *cs, gpio_t *dc, int w, int h);
void st7735_set_background_color(st7735_t *self, st7735_color_t c);
void st7735_set_offset(st7735_t *self, int offset_x, int offset_y);
void st7735_set_orientation(st7735_t *self, st7735_orientation_t orientation);

// Generic functions
void st7735_clear_screen(st7735_t *self);
void st7735_draw_pixel(st7735_t *self, int x, int y, st7735_color_t c);
void st7735_fill_rectangle(st7735_t *self, int x, int y, int w, int h, st7735_color_t c);

// Strings and characters
void st7735_draw_char(st7735_t *self, int x, int y, const char chr, st7735_color_t color, int scale);
void st7735_draw_chars(st7735_t *self, int x, int y, const char* chars, int length, st7735_color_t color, int scale);
void st7735_draw_string(st7735_t *self, int x, int y, const char* str, st7735_color_t color, int scale);
void st7735_clear_char(st7735_t *self, int x, int y, int scale);
void st7735_clear_chars(st7735_t *self, int x, int y, int length, int scale);

// Graphics
void st7735_render(st7735_t *self, int x, int y, int w, int h, ST7735_Renderer renderer, void* param, int scale);
void st7735_draw_xbm(st7735_t *self, st7735_xbm_t *bits, int x, int y, int w, int h, st7735_color_t c, int scale);

// Valid for 16bits 565 mode
#define ST7735_RED(x)    ((((x) >> 3) & 0x1F) << 11)
#define ST7735_GREEN(x)  ((((x) >> 2) & 0x3F) << 5)
#define ST7735_BLUE(x)   (((x) >> 3) & 0x1F)

#define ST7735_COLOR_BLACK    0x0000
#define ST7735_COLOR_WHITE    0xFFFF
#define ST7735_COLOR_RED      ST7735_RED(0xFF)
#define ST7735_COLOR_LIME     ST7735_GREEN(0xFF)
#define ST7735_COLOR_BLUE     ST7735_BLUE(0xFF)
#define ST7735_COLOR_MAROON   ST7735_RED(0x80)
#define ST7735_COLOR_GREEN    ST7735_GREEN(0x80)
#define ST7735_COLOR_NAVY     ST7735_BLUE(0x80)
#define ST7735_COLOR_YELLOW   ST7735_RED(0xFF) | ST7735_GREEN(0xFF)
#define ST7735_COLOR_FUSCHIA  ST7735_RED(0xFF) | ST7735_BLUE(0xFF)
#define ST7735_COLOR_PINK     ST7735_RED(0xFF) | ST7735_GREEN(0xC0) | ST7735_BLUE(0xC0)
#define ST7735_COLOR_CYAN     ST7735_GREEN(0xFF) | ST7735_BLUE(0xFF)
#define ST7735_COLOR_PURPLE   ST7735_RED(0x80) | ST7735_BLUE(0x80)
#define ST7735_COLOR_OLIVE    ST7735_RED(0x80) | ST7735_GREEN(0x80)
#define ST7735_COLOR_TEAL     ST7735_GREEN(0x80) | ST7735_BLUE(0x80)
#define ST7735_COLOR_SILVER   ST7735_RED(0xC0) | ST7735_GREEN(0xC0) | ST7735_BLUE(0xC0)
#define ST7735_COLOR_GREY     ST7735_RED(0x80) | ST7735_GREEN(0x80) | ST7735_BLUE(0x80)
#define ST7735_COLOR_DARKGREY ST7735_RED(0x40) | ST7735_GREEN(0x40) | ST7735_BLUE(0x40)
#define ST7735_COLOR_ORANGE   ST7735_RED(0xFF) | ST7735_GREEN(0x80)
#define ST7735_COLOR_SPRING   ST7735_GREEN(0xFF) | ST7735_BLUE(0x80)

#endif // SRC_ST7735_API_ST7735_H_
