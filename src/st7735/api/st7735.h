#ifndef SRC_ST7735_API_ST7735_H_
#define SRC_ST7735_API_ST7735_H_

#include "spi.h"
#include "st7735_cfg.h"
#include "types.h"

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

typedef uint16_t st7735_color_t;
typedef const __flash uint8_t st7735_xbm_t;
typedef char* st7735_xpm_t;
typedef st7735_color_t (*ST7735_Renderer)(int x, int y, int w, int h, void* data);

void ST7735_Init();
void ST7735_ClearScreen();
void ST7735_SetBackgroundColor(st7735_color_t c);

// Pixel drawing
void ST7735_DrawPixel(int x, int y, st7735_color_t c);
void ST7735_FillRectangle(int x, int y, int w, int h, st7735_color_t c);
void ST7735_DrawLine(int x1, int y1, int x2, int y2, st7735_color_t c);


void ST7735_ClearChar(int x, int y, int scale);
void ST7735_ClearChars(int x, int y, int length, int scale);

void ST7735_DrawChar(int x, int y, const char chr, st7735_color_t color, int scale);
void ST7735_DrawChars(int x, int y, const char* buf, int len, st7735_color_t color, int scale);
void ST7735_DrawString(int x, int y, const char* str, st7735_color_t color, int scale);

// Imaging
void ST7735_Render(int x, int y, int w, int h, ST7735_Renderer renderer, void* param, int scale);
void ST7735_DrawXBM(st7735_xbm_t *bits, int x, int y, int width, int height, st7735_color_t c, int scale);
Std_ReturnType ST7735_DrawXPM(st7735_xpm_t *xpm, int x, int y, int scale);

#endif // SRC_ST7735_API_ST7735_H_
