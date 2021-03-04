#ifndef SRC_ST7735_API_ST7735_PRV_H_
#define SRC_ST7735_API_ST7735_PRV_H_

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

#define ST7735_MADCTL_MY BIT(7)
#define ST7735_MADCTL_MX BIT(6)
#define ST7735_MADCTL_MV BIT(5)

#define ST7735_COLMOD_12_BPP 3
#define ST7735_COLMOD_16_BPP 5
#define ST7735_COLMOD_18_BPP 6

// Renderers
static st7735_color_t ST7735_RenderXbm(int x, int y, int w, int h, void *data);

typedef struct {
    st7735_xbm_t *bits;
    st7735_color_t fg_color;
    st7735_color_t bg_color;
    const int bw;
} XbmRendererData;

#endif // SRC_ST7735_API_ST7735_PRV_H_
