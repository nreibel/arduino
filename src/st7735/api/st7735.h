#ifndef SRC_ST7735_API_ST7735_H_
#define SRC_ST7735_API_ST7735_H_

#include "st7735_cfg.h"
#include "types.h"
#include "port.h"

#define ST7735_CHARSET_WIDTH  5
#define ST7735_CHARSET_HEIGHT 7

// Valid for 16bits 565 mode
#define ST7735_RED(x)    ((((x) >> 3) & 0x1F) << 11)
#define ST7735_GREEN(x)  ((((x) >> 2) & 0x3F) << 5)
#define ST7735_BLUE(x)   (((x) >> 3) & 0x1F)

#define ST7735_COLOR_BLACK   0x0000
#define ST7735_COLOR_WHITE   0xFFFF
#define ST7735_COLOR_RED     ST7735_RED(0xFF)
#define ST7735_COLOR_LIME    ST7735_GREEN(0xFF)
#define ST7735_COLOR_BLUE    ST7735_BLUE(0xFF)
#define ST7735_COLOR_MAROON  ST7735_RED(0x80)
#define ST7735_COLOR_GREEN   ST7735_GREEN(0x80)
#define ST7735_COLOR_NAVY    ST7735_BLUE(0x80)
#define ST7735_COLOR_YELLOW  ST7735_RED(0xFF) | ST7735_GREEN(0xFF)
#define ST7735_COLOR_FUSCHIA ST7735_RED(0xFF) | ST7735_BLUE(0xFF)
#define ST7735_COLOR_CYAN    ST7735_GREEN(0xFF) | ST7735_BLUE(0xFF)
#define ST7735_COLOR_PURPLE  ST7735_RED(0x80) | ST7735_BLUE(0x80)
#define ST7735_COLOR_OLIVE   ST7735_RED(0x80) | ST7735_GREEN(0x80)
#define ST7735_COLOR_TEAL    ST7735_GREEN(0x80) | ST7735_BLUE(0x80)
#define ST7735_COLOR_SILVER  ST7735_RED(0xC0) | ST7735_GREEN(0xC0) | ST7735_BLUE(0xC0)
#define ST7735_COLOR_GREY    ST7735_RED(0x80) | ST7735_GREEN(0x80) | ST7735_BLUE(0x80)
#define ST7735_COLOR_ORANGE  ST7735_RED(0xFF) | ST7735_GREEN(0x80)
#define ST7735_COLOR_SPRING  ST7735_GREEN(0xFF) | ST7735_BLUE(0x80)

#define ST7735_SCREEN_ORIENTATION_LANDSCAPE     ST7735_MADCTL_MV | ST7735_MADCTL_MY
#define ST7735_SCREEN_ORIENTATION_LANDSCAPE_INV ST7735_MADCTL_MV | ST7735_MADCTL_MX
#define ST7735_SCREEN_ORIENTATION_PORTRAIT      0x0
#define ST7735_SCREEN_ORIENTATION_PORTRAIT_INV  ST7735_MADCTL_MY | ST7735_MADCTL_MX

void ST7735_Init();

// Pixel drawing
void ST7735_DrawPixel(const uint8_t x, const uint8_t y, const uint16_t color);
void ST7735_FillRectangle(const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h, const uint16_t color);
void ST7735_DrawLine(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2, const uint16_t color);

// String drawing
void ST7735_CharTest(void);
void ST7735_ClearChar(const uint8_t x, const uint8_t y, const uint16_t backgroundColor);
void ST7735_DrawChar(const uint8_t x, const uint8_t y, const char c, const uint16_t foregroundColor, const uint16_t backgroundColor);

void ST7735_ClearString(const uint8_t x, const uint8_t y, const int length, const uint16_t backgroundColor);
void ST7735_DrawString(const uint8_t x, const uint8_t y, const char* str, const uint16_t foregroundColor, const uint16_t backgroundColor);

// Imaging
void ST7735_DrawXBM(const uint8_t *bits, const uint8_t xPos, const uint8_t yPos, const uint8_t width, const uint8_t height, const uint16_t foregroundColor, const uint16_t backgroundColor);

#endif // SRC_ST7735_API_ST7735_H_
