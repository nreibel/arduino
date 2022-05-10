#include "st7735.h"
#include "st7735_cfg.h"
#include "spi.h"
#include "gpio.h"
#include "bits.h"
#include "charset.h"
#include "os.h"

/*
 * Private constants
 */

#if ST7735_SCALING_ENABLED
#define SCALE self->scale
#else
#define SCALE 1
#endif

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

/*
 * Private functions declaration
 */

static void st7735_command(st7735_t self, uint8_t command);
static void st7735_data(st7735_t self, uint8_t data);
static void st7735_color(st7735_t self, st7735_color_t color);
static void st7735_set_draw_window(st7735_t self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

/*
 * Renderers
 */

static st7735_color_t ST7735_RenderXbm(unsigned int x, unsigned int y, unsigned int w, unsigned int h, void *data);

typedef struct {
    st7735_xbm_t *bits;
    st7735_color_t fg_color;
    st7735_color_t bg_color;
} XbmRendererData;

/*
 * Object mehtods
 */

void st7735_init_device(st7735_t self, spi_bus_t bus, gpio_t cs, gpio_t dc, unsigned int w, unsigned int h)
{
    spi_device_init(&self->dev, bus, cs, SPI_CLOCK_DIV_2, SPI_MODE_0);
    spi_device_set_transaction_mode(&self->dev, true);
    spi_device_enable(&self->dev);

    gpio_configure(dc, GPIO_OUTPUT_ACTIVE_LOW);

    self->width = w;
    self->height = h;
    self->offset_x = 0;
    self->offset_y = 0;
    self->dc = dc;
    self->background_color = ST7735_COLOR_BLACK;
    self->foreground_color = ST7735_COLOR_WHITE;
    self->scale = 1;

    // TFT startup routine
    st7735_command(self, ST7735_SWRESET);
    os_wait(120);
    st7735_command(self, ST7735_SLPOUT);
    os_wait(120);

    // 16bits per pixel
    st7735_command(self, ST7735_COLMOD);
    st7735_data(self, ST7735_COLMOD_16_BPP);

    // Screen orientation
    st7735_command(self, ST7735_MADCTL);
    st7735_data(self, ST7735_ORIENTATION_PORTRAIT);

    st7735_command(self, ST7735_DISPON);
    spi_device_disable(&self->dev);
}

unsigned int st7735_get_width(st7735_t self)
{
    return self->width;
}

unsigned int st7735_get_height(st7735_t self)
{
    return self->height;
}

void st7735_set_scale(st7735_t self, unsigned int scale)
{
    self->scale = scale;
}

static void st7735_data(st7735_t self, uint8_t data)
{
    spi_bus_t bus = spi_device_get_bus(&self->dev);
    spi_bus_write_fast(bus, data);
}

static void st7735_command(st7735_t self, uint8_t command)
{
    gpio_set(self->dc);
    spi_bus_t bus = spi_device_get_bus(&self->dev);
    spi_bus_write_fast(bus, command);
    gpio_reset(self->dc);
}

static void st7735_color(st7735_t self, st7735_color_t color)
{
    spi_bus_t bus = spi_device_get_bus(&self->dev);
    spi_bus_write_fast(bus, color.b[1]);
    spi_bus_write_fast(bus, color.b[0]);
}

static void st7735_set_draw_window(st7735_t self, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
    spi_bus_t bus = spi_device_get_bus(&self->dev);

    // Set the column to write to
    st7735_command(self, ST7735_CASET);
    spi_bus_write_fast(bus, 0);
    spi_bus_write_fast(bus, x1+self->offset_x);
    spi_bus_write_fast(bus, 0);
    spi_bus_write_fast(bus, x2+self->offset_x);

    // Set the row range to write to
    st7735_command(self, ST7735_RASET);
    spi_bus_write_fast(bus, 0);
    spi_bus_write_fast(bus, y1+self->offset_y);
    spi_bus_write_fast(bus, 0);
    spi_bus_write_fast(bus, y2+self->offset_y);

    // Write to RAM
    st7735_command(self, ST7735_RAMWR);
}

void st7735_set_orientation(st7735_t self, st7735_orientation_t orientation)
{
    uint8_t madctl = 0;

    switch(orientation)
    {
        case ST7735_ORIENTATION_PORTRAIT:
            madctl = 0;
            break;
        case ST7735_ORIENTATION_LANDSCAPE:
            madctl = ST7735_MADCTL_MV | ST7735_MADCTL_MY;
            break;
        case ST7735_ORIENTATION_PORTRAIT_INV:
            madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MX;
            break;
        case ST7735_ORIENTATION_LANDSCAPE_INV:
            madctl = ST7735_MADCTL_MV | ST7735_MADCTL_MX;
            break;
    }

    spi_device_enable(&self->dev);
    st7735_command(self, ST7735_MADCTL);
    st7735_data(self, madctl);
    spi_device_disable(&self->dev);
}

void st7735_set_background(st7735_t self, st7735_color_t c)
{
    self->background_color = c;
}

void st7735_set_foreground(st7735_t self, st7735_color_t c)
{
    self->foreground_color = c;
}

st7735_color_t st7735_get_foreground(st7735_t self)
{
    return self->foreground_color;
}

st7735_color_t st7735_get_background(st7735_t self)
{
    return self->background_color;
}

void st7735_set_offset(st7735_t self, int offset_x, int offset_y)
{
    self->offset_x = offset_x;
    self->offset_y = offset_y;
}

void st7735_clear_screen(st7735_t self)
{
    st7735_color_t fg = st7735_get_foreground(self);
    st7735_color_t bg = st7735_get_background(self);

    st7735_set_foreground(self, bg);
    st7735_fill_rectangle(self, 0, 0, self->width, self->height);
    st7735_set_foreground(self, fg);
}

void st7735_draw_pixel(st7735_t self, unsigned int x, unsigned int y)
{
    st7735_fill_rectangle(self, x, y, 1, 1);
}

void st7735_fill_rectangle(st7735_t self, unsigned int px, unsigned int py, unsigned int w, unsigned int h)
{
    spi_device_enable(&self->dev);

    st7735_set_draw_window(self, px, py, px+w-1, py+h-1);

    for(unsigned int i = 0; i < w*h; i++)
    {
        st7735_color(self, self->foreground_color);
    }

    spi_device_disable(&self->dev);
}

void st7735_draw_char(st7735_t self, unsigned int x, unsigned int y, const char chr)
{
    const __flash uint8_t *raw = NULL_PTR;

    if ( BETWEEN(chr, ' ', '~') )
    {
        raw = s_st7735_charset[chr-0x20];
    }
#if ST7735_EXTENDED_ASCII
    else if ( (uint8_t) chr >= 0x80 )
    {
        raw = s_st7735_charset_ext[ (uint8_t) chr - ST7735_EXT_ASCII_START_IDX ];
    }
#endif // ST7735_EXTENDED_ASCII
    else return;

    unsigned int w = SCALE * ST7735_CHARSET_WIDTH;
    unsigned int h = SCALE * ST7735_CHARSET_HEIGHT;

    spi_device_enable(&self->dev);
    st7735_set_draw_window(self, x, y, x+w-1, y+h-1);

    for (unsigned int dy = 0 ; dy < ST7735_CHARSET_HEIGHT ; dy++)
    {
        for (unsigned int i = 0 ; i < SCALE ; i++)
        {
            for (unsigned int dx = 0 ; dx < ST7735_CHARSET_WIDTH ; dx++)
            {
                uint8_t b = raw[dx];
                st7735_color_t px = IS_SET_BIT(b, dy) ? self->foreground_color : self->background_color;

                for (unsigned int j = 0 ; j < SCALE ; j++)
                {
                    st7735_color(self, px);
                }
            }
        }
    }

    spi_device_disable(&self->dev);
}

void st7735_draw_chars(st7735_t self, unsigned int x, unsigned int y, const char* chars, int length)
{
    for (int i = 0 ; i < length ; i++)
    {
        st7735_draw_char(self, x, y, chars[i]);
        x += SCALE * (ST7735_CHARSET_WIDTH + 1);
    }
}

void st7735_draw_string(st7735_t self, unsigned int x, unsigned int y, const char* str)
{
    for(int i = 0 ; str[i] != 0 ; i++)
    {
        st7735_draw_char(self, x, y, str[i]);
        x += SCALE * (ST7735_CHARSET_WIDTH + 1);
    }
}

void st7735_clear_char(st7735_t self, unsigned int x, unsigned int y)
{
    st7735_color_t fg = st7735_get_foreground(self);
    st7735_color_t bg = st7735_get_background(self);

    st7735_set_foreground(self, bg);
    st7735_fill_rectangle(self, x, y, SCALE * ST7735_CHARSET_WIDTH, SCALE * ST7735_CHARSET_HEIGHT);
    st7735_set_foreground(self, fg);
}

void st7735_clear_chars(st7735_t self, unsigned int x, unsigned int y, int length)
{
    for (int i = 0 ; i < length ; i++)
    {
        st7735_clear_char(self, x, y);
        x += SCALE * (ST7735_CHARSET_WIDTH + 1);
    }
}

static st7735_color_t ST7735_RenderXbm(unsigned int x, unsigned int y, unsigned int w, unsigned int h, void *data)
{
    UNUSED(h);
    UNUSED(w);

    XbmRendererData *d = TYPECAST(data, XbmRendererData*);
    uint8_t bw = (w+7)/8; // Number of bytes per line
    uint8_t b = d->bits[y*bw + x/8];
    return IS_SET_BIT(b, x % 8) ? d->fg_color : d->bg_color;
}

void st7735_draw_xbm(st7735_t self, st7735_xbm_t *bits, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    XbmRendererData data = { bits, self->foreground_color, self->background_color };
    st7735_render(self, x, y, w, h, ST7735_RenderXbm, &data);
}

void st7735_render(st7735_t self, unsigned int px, unsigned int py, unsigned int w, unsigned int h, ST7735_Renderer renderer, void* param)
{
    //Set the drawing region
    spi_device_enable(&self->dev);
    st7735_set_draw_window(self, px, py, px+(SCALE*w)-1, py+(SCALE*h)-1);

    for(unsigned int y = 0; y < h; y++)
    {
        for(unsigned int i = 0; i < SCALE; i++)
        {
            for(unsigned int x = 0; x < w; x++)
            {
                st7735_color_t c = renderer(x, y, w, h, param);

                for(unsigned int j = 0; j < SCALE; j++)
                {
                    st7735_color(self, c);
                }
            }
        }
    }

    spi_device_disable(&self->dev);
}

// Std_ReturnType ST7735_DrawXPM(st7735_xpm_t *xpm, int xPos, int yPos)
// {
//     int width, height, nbColors, sz;
//     st7735_color_t colors[16];
//
//     // Read image attributes
//     if ( sscanf(xpm[0], "%d %d %d %d", &width, &height, &nbColors, &sz) != 4 ) return Status_Not_OK;
//
//     // Can't handle more than 1 byte color indices
//     if (sz != 1) return Status_Not_OK;
//
//     // Can't handle more than 16 colors
//     if (nbColors > 16) return Status_Not_OK;
//
//     // Read all colors
//     for (int i = 0 ; i < nbColors ; i++)
//     {
//         char name, chr;
//         int r, g, b;
//
//         if ( sscanf(xpm[1+i], "%c c #%2x%2x%2x", &name, &r, &g, &b) == 4 )
//         {
//             // Actual color
//             colors[i] = ST7735_RED(r) | ST7735_GREEN(g) | ST7735_BLUE(b);
//         }
//         else if ( sscanf(xpm[1+i], "%c c Non%c", &name, &chr) == 2 && chr == 'e')
//         {
//             // Transparent color
//             colors[i] = background_color;
//         }
//         else return Status_Not_OK;
//
//         // Replace in image data all names with color index
//         for (int x = 0 ; x < height ; x++)
//         {
//             for (int y = 0 ; y < width ; y++)
//             {
//                 if (xpm[1 + nbColors + x][y] == name)
//                 {
//                     xpm[1 + nbColors + x][y] = i;
//                 }
//             }
//         }
//     }
//
//     // Draw the image
//     ST7735_SetDrawWindow(xPos, yPos, xPos+(width*scale)-1, yPos+(height*scale)-1);
//     for (int x = 0 ; x < height ; x++)
//     {
//         // Repeat each line with scale factor
//         for (int _s1 = 0 ; _s1 < scale ; _s1++)
//         {
//             for (int y = 0 ; y < width ; y++)
//             {
//                 // Repeat each pixel with scale factor
//                 int idx = xpm[1 + nbColors + x][y];
//                 for (int _s2 = 0 ; _s2 < scale ; _s2++)
//                 {
//                     ST7735_Color( colors[idx] );
//                 }
//             }
//         }
//     }
//
//     return Status_OK;
// }
//
// void ST7735_DrawLine(int x1, int y1, int x2, int y2, st7735_color_t c)
// {
//     // Integer only implementation of Bresenham's algorythm
//     int CurrentX = x1, CurrentY = y1;
//     int Xinc = 1, Yinc = 1;
//     int Dx = x2-x1;
//     int Dy = y2-y1;
//
//     if (Dx < 0)
//     {
//         Xinc = -1;
//         Dx = -Dx;
//     }
//
//     if (Dy < 0)
//     {
//         Yinc = -1;
//         Dy = -Dy;
//     }
//
//     ST7735_DrawPixel(x1, y1, c);
//
//     if (Dy <= Dx)
//     {
//         int TwoDxAccumulatedError = 0;
//         do
//         {
//             CurrentX += Xinc;
//             TwoDxAccumulatedError += 2*Dy;
//             if (TwoDxAccumulatedError > Dx)
//             {
//                 CurrentY += Yinc;
//                 TwoDxAccumulatedError -= 2*Dx;
//             }
//
//             ST7735_DrawPixel(CurrentX, CurrentY, c);
//         }
//         while(CurrentX != x2);
//     }
//     else
//     {
//         int TwoDyAccumulatedError = 0;
//         do
//         {
//             CurrentY += Yinc;
//             TwoDyAccumulatedError += 2*Dx;
//             if (TwoDyAccumulatedError > Dy)
//             {
//                 CurrentX += Xinc;
//                 TwoDyAccumulatedError -= 2*Dy;
//             }
//
//             ST7735_DrawPixel(CurrentX, CurrentY, c);
//         }
//         while(CurrentY != y2);
//     }
//
//     ST7735_DrawPixel(x2, y2, c);
// }
