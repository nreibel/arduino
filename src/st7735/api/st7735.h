#ifndef SRC_ST7735_API_ST7735_H_
#define SRC_ST7735_API_ST7735_H_

#include "st7735_cfg.h"
#include "types.h"
#include "spi.h"
#include "gpio.h"

typedef uint16_t st7735_color_t;
typedef const __flash uint8_t st7735_xbm_t;
// typedef char* st7735_xpm_t;
typedef st7735_color_t (*ST7735_Renderer)(unsigned int x, unsigned int y, unsigned int w, unsigned int h, void* data);

typedef struct st7735_prv_s {
    struct spi_device_prv_s dev;
    st7735_color_t background_color;
    st7735_color_t foreground_color;
#if ST7735_SCALING_ENABLED
    unsigned int scale;
#endif // ST7735_SCALING_ENABLED
    gpio_t dc;
    uint8_t width;
    uint8_t height;
    int8_t offset_x;
    int8_t offset_y;
} * st7735_t;

typedef enum {
    ST7735_ORIENTATION_PORTRAIT,
    ST7735_ORIENTATION_LANDSCAPE,
    ST7735_ORIENTATION_PORTRAIT_INV,
    ST7735_ORIENTATION_LANDSCAPE_INV
} st7735_orientation_t;

void st7735_init_device(st7735_t self, spi_bus_t bus, gpio_t cs, gpio_t dc, unsigned int w, unsigned int h);

// Setters
void st7735_set_foreground(st7735_t self, st7735_color_t c);
void st7735_set_background(st7735_t self, st7735_color_t c);
void st7735_set_offset(st7735_t self, int offset_x, int offset_y);
void st7735_set_orientation(st7735_t self, st7735_orientation_t orientation);

#if ST7735_SCALING_ENABLED
void st7735_set_scale(st7735_t self, unsigned int scale);
#endif // ST7735_SCALING_ENABLED

// Getters
unsigned int st7735_get_width(st7735_t self);
unsigned int st7735_get_height(st7735_t self);
st7735_color_t st7735_get_foreground(st7735_t self);
st7735_color_t st7735_get_background(st7735_t self);

// Generic functions
void st7735_clear_screen(st7735_t self);
void st7735_draw_pixel(st7735_t self, unsigned int x, unsigned int y);
void st7735_fill_rectangle(st7735_t self, unsigned int x, unsigned int y, unsigned int w, unsigned int h);

// Strings and characters
void st7735_draw_char   (st7735_t self, unsigned int x, unsigned int y, const char chr);
void st7735_draw_chars  (st7735_t self, unsigned int x, unsigned int y, const char* chars, int length);
void st7735_draw_string (st7735_t self, unsigned int x, unsigned int y, const char* str);
void st7735_clear_char  (st7735_t self, unsigned int x, unsigned int y);
void st7735_clear_chars (st7735_t self, unsigned int x, unsigned int y, int length);

// Graphics
void st7735_render(st7735_t self, unsigned int x, unsigned int y, unsigned int w, unsigned int h, ST7735_Renderer renderer, void* param);
void st7735_draw_xbm(st7735_t self, st7735_xbm_t *bits, unsigned int x, unsigned int y, unsigned int w, unsigned int h);

// Valid for 16bits 565 mode
#define ST7735_RED(x)    ((((x) >> 3) & 0x1FU) << 11)
#define ST7735_GREEN(x)  ((((x) >> 2) & 0x3FU) << 5)
#define ST7735_BLUE(x)   (((x) >> 3) & 0x1FU)

#define ST7735_COLOR_BLACK    0x0000U
#define ST7735_COLOR_WHITE    0xFFFFU
#define ST7735_COLOR_RED      ST7735_RED(0xFFU)
#define ST7735_COLOR_LIME     ST7735_GREEN(0xFFU)
#define ST7735_COLOR_BLUE     ST7735_BLUE(0xFFU)
#define ST7735_COLOR_MAROON   ST7735_RED(0x80U)
#define ST7735_COLOR_GREEN    ST7735_GREEN(0x80U)
#define ST7735_COLOR_NAVY     ST7735_BLUE(0x80U)
#define ST7735_COLOR_YELLOW   ST7735_RED(0xFFU) | ST7735_GREEN(0xFFU)
#define ST7735_COLOR_FUSCHIA  ST7735_RED(0xFFU) | ST7735_BLUE(0xFFU)
#define ST7735_COLOR_PINK     ST7735_RED(0xFFU) | ST7735_GREEN(0xC0U) | ST7735_BLUE(0xC0U)
#define ST7735_COLOR_CYAN     ST7735_GREEN(0xFFU) | ST7735_BLUE(0xFFU)
#define ST7735_COLOR_PURPLE   ST7735_RED(0x80U) | ST7735_BLUE(0x80U)
#define ST7735_COLOR_OLIVE    ST7735_RED(0x80U) | ST7735_GREEN(0x80U)
#define ST7735_COLOR_TEAL     ST7735_GREEN(0x80U) | ST7735_BLUE(0x80U)
#define ST7735_COLOR_SILVER   ST7735_RED(0xC0U) | ST7735_GREEN(0xC0U) | ST7735_BLUE(0xC0U)
#define ST7735_COLOR_GREY     ST7735_RED(0x80U) | ST7735_GREEN(0x80U) | ST7735_BLUE(0x80U)
#define ST7735_COLOR_DARKGREY ST7735_RED(0x40U) | ST7735_GREEN(0x40U) | ST7735_BLUE(0x40U)
#define ST7735_COLOR_ORANGE   ST7735_RED(0xFFU) | ST7735_GREEN(0x80U)
#define ST7735_COLOR_SPRING   ST7735_GREEN(0xFFU) | ST7735_BLUE(0x80U)

#endif // SRC_ST7735_API_ST7735_H_
