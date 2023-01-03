#ifndef __ST7735_API_H
#define __ST7735_API_H

#include "types.h"
#include "gpio.h"
#include "spi.h"
#include "charset.h"
#include "os_cfg.h"
#include "st7735_cfg.h"

typedef enum {
    ST7735_ORIENTATION_PORTRAIT,
    ST7735_ORIENTATION_LANDSCAPE,
    ST7735_ORIENTATION_PORTRAIT_INV,
    ST7735_ORIENTATION_LANDSCAPE_INV
} st7735_orientation_t;

typedef union {
    struct {
        uint16_t b : 5;
        uint16_t g : 6;
        uint16_t r : 5;
    } c;
    uint8_t  b[2];
    uint16_t w;
} st7735_color_t;

typedef const __flash uint8_t st7735_xbm_t;
// typedef char* st7735_xpm_t;
typedef st7735_color_t (*ST7735_Renderer)(unsigned int x, unsigned int y, unsigned int w, unsigned int h, void* data);

typedef struct st7735_prv_s {
    struct spi_device_prv_s dev;
    gpio_t                  dc;
    st7735_color_t          background_color;
    st7735_color_t          foreground_color;
    unsigned int            scale;
    st7735_orientation_t    orientation;
    int8_t                  offset_x;
    int8_t                  offset_y;
} * st7735_t;

#if OS_MALLOC
st7735_t st7735_create(void);
void st7735_destroy(st7735_t dev);
#endif // OS_MALLOC

void st7735_init(st7735_t self, spi_bus_t bus, gpio_t cs, gpio_t dc);

// Setters
void st7735_set_foreground(st7735_t self, st7735_color_t c);
void st7735_set_background(st7735_t self, st7735_color_t c);
void st7735_set_offset(st7735_t self, int offset_x, int offset_y);
void st7735_set_orientation(st7735_t self, st7735_orientation_t orientation);

#if ST7735_SCALING_ENABLED
void st7735_set_scale(st7735_t self, unsigned int scale);
#endif // ST7735_SCALING_ENABLED

// Getters
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
#define ST7735_COLOR(_r, _g, _b) ((st7735_color_t){.c={.r=_r>>3,.g=_g>>2,.b=_b>>3}})

#define ST7735_COLOR_BLACK    ST7735_COLOR(0,    0,    0   )
#define ST7735_COLOR_DARKGREY ST7735_COLOR(0x40, 0x40, 0x40)
#define ST7735_COLOR_GREY     ST7735_COLOR(0x80, 0x80, 0x80)
#define ST7735_COLOR_SILVER   ST7735_COLOR(0xC0, 0xC0, 0xC0)
#define ST7735_COLOR_WHITE    ST7735_COLOR(0xFF, 0xFF, 0xFF)
#define ST7735_COLOR_RED      ST7735_COLOR(0xFF, 0,    0   )
#define ST7735_COLOR_LIME     ST7735_COLOR(0,    0xFF, 0   )
#define ST7735_COLOR_BLUE     ST7735_COLOR(0,    0,    0xFF)
#define ST7735_COLOR_MAROON   ST7735_COLOR(0x80, 0,    0   )
#define ST7735_COLOR_GREEN    ST7735_COLOR(0,    0x80, 0   )
#define ST7735_COLOR_NAVY     ST7735_COLOR(0,    0,    0x80)
#define ST7735_COLOR_CYAN     ST7735_COLOR(0,    0xFF, 0xFF)
#define ST7735_COLOR_FUSCHIA  ST7735_COLOR(0xFF, 0,    0xFF)
#define ST7735_COLOR_YELLOW   ST7735_COLOR(0xFF, 0xFF, 0   )
#define ST7735_COLOR_TEAL     ST7735_COLOR(0,    0x80, 0x80)
#define ST7735_COLOR_PURPLE   ST7735_COLOR(0x80, 0,    0x80)
#define ST7735_COLOR_OLIVE    ST7735_COLOR(0x80, 0x80, 0   )
#define ST7735_COLOR_ORANGE   ST7735_COLOR(0xFF, 0x80, 0   )
#define ST7735_COLOR_SPRING   ST7735_COLOR(0,    0xFF, 0x80)
#define ST7735_COLOR_VIOLET   ST7735_COLOR(0x80, 0,    0xFF)
#define ST7735_COLOR_PINK     ST7735_COLOR(0xFF, 0xC0, 0xC0)

#endif // __ST7735_API_H
