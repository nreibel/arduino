#ifndef SRC_ST7735_API_ST7735_PRV_H_
#define SRC_ST7735_API_ST7735_PRV_H_

#include "st7735.h"
#include "st7735_cfg.h"

//Command definitions
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E
#define ST7735_MADCTL  0x36
#define ST7735_COLMOD  0x3A

#define ST7735_MADCTL_MY 0x80
#define ST7735_MADCTL_MX 0x40
#define ST7735_MADCTL_MV 0x20

extern GPIO ST7735_Pin_DC; // Data/command pin
extern GPIO ST7735_Pin_RS; // Reset pin

void ST7735_Data(uint8_t data);
void ST7735_Command(uint8_t command);
void ST7735_SetDrawWindow(int x1, int y1, int x2, int y2);

#endif // SRC_ST7735_API_ST7735_PRV_H_
