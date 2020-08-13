#ifndef SRC_ST7735_API_ST7735_H_
#define SRC_ST7735_API_ST7735_H_

#include "spi.h"
#include "st7735_cfg.h"
#include "types.h"
#include "port.h"

#define ST7735_CHARSET_WIDTH  5
#define ST7735_CHARSET_HEIGHT 7

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

#define ST7735_SCREEN_ORIENTATION_LANDSCAPE     ST7735_MADCTL_MV | ST7735_MADCTL_MY
#define ST7735_SCREEN_ORIENTATION_LANDSCAPE_INV ST7735_MADCTL_MV | ST7735_MADCTL_MX
#define ST7735_SCREEN_ORIENTATION_PORTRAIT      0x0
#define ST7735_SCREEN_ORIENTATION_PORTRAIT_INV  ST7735_MADCTL_MY | ST7735_MADCTL_MX

typedef uint16_t (*ST7735_Renderer)(int x, int y, int w, int h, void* data);

void ST7735_Init();
void ST7735_ClearScreen(uint16_t color);

// Pixel drawing
void ST7735_DrawPixel(int x, int y, uint16_t color);
void ST7735_FillRectangle(int x, int y, int w, int h, uint16_t color);
void ST7735_DrawLine(int x1, int y1, int x2, int y2, uint16_t color);

// String and chars drawing
void ST7735_CharTest(void);

void ST7735_ClearChar(int x, int y, uint16_t backgroundColor);
void ST7735_ClearChars(int x, int y, int length, uint16_t backgroundColor);

void ST7735_DrawChar(int x, int y, char c, uint16_t foregroundColor, uint16_t backgroundColor);
void ST7735_DrawChars(int x, int y, char *bytes, int length, uint16_t foregroundColor, uint16_t backgroundColor);

void ST7735_DrawString(int x, int y, char *str, uint16_t foregroundColor, uint16_t backgroundColor);

// Imaging
void ST7735_Render(int x, int y, int w, int h, ST7735_Renderer renderer, void* param, int scale);
void ST7735_DrawXBM(const __flash uint8_t *bits, int x, int y, int width, int height, uint16_t foregroundColor, uint16_t backgroundColor, int scale);
Std_ReturnType ST7735_DrawXPM(char *xpm[], int x, int y, uint16_t bgColor, int scale);

#endif // SRC_ST7735_API_ST7735_H_
