#include "st7735.h"
#include "st7735_prv.h"
#include "spi.h"
#include "gpio.h"
#include "bits.h"
#include "charset.h"
#include "os.h"

static void st7735_command(st7735_t *self, uint8_t command);
static void st7735_data(st7735_t *self, uint8_t data);
static void st7735_color(st7735_t *self, st7735_color_t color);
static void st7735_set_draw_window(st7735_t *self, int x1, int y1, int x2, int y2);

void st7735_init_device(st7735_t *self, gpio_t *cs, gpio_t *dc, int w, int h)
{
    spi_device_init(&self->spi_device, cs, SPI_CLOCK_DIV_2, SPI_MODE_0);
    spi_set_transaction_mode_enabled(&self->spi_device, TRUE);
    spi_enable_slave(&self->spi_device);

    self->width = w;
    self->height = h;
    self->dc = dc;
    self->background_color = ST7735_COLOR_BLACK;

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
    st7735_data(self, 0);

    st7735_command(self, ST7735_DISPON);
    spi_disable_slave(&self->spi_device);
}

static void st7735_data(st7735_t *self, uint8_t data)
{
    spi_write_byte(&self->spi_device, data, NULL_PTR);
}

static void st7735_command(st7735_t *self, uint8_t command)
{
    gpio_set(self->dc);
    spi_write_byte(&self->spi_device, command, NULL_PTR);
    gpio_reset(self->dc);
}

static void st7735_color(st7735_t *self, st7735_color_t color)
{
    // Swap bytes
    word w = { (color << 8) | (color >> 8) };
    spi_write_bytes(&self->spi_device, w.bytes, 2);
}

static void st7735_set_draw_window(st7735_t *self, int x1, int y1, int x2, int y2)
{
    // Set the column to write to
    uint8_t col[4] = {0, x1+self->offset_x, 0, x2+self->offset_x};
    st7735_command(self, ST7735_CASET);
    spi_write_bytes(&self->spi_device, col, 4);

    // Set the row range to write to
    uint8_t row[4] = {0, y1+self->offset_y, 0, y2+self->offset_y};
    st7735_command(self, ST7735_RASET);
    spi_write_bytes(&self->spi_device, row, 4);

    // Write to RAM
    st7735_command(self, ST7735_RAMWR);
}

void st7735_set_orientation(st7735_t *self, st7735_orientation_t orientation)
{
    uint8_t madctl = 0;

    switch(orientation) {
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

    spi_enable_slave(&self->spi_device);
    st7735_command(self, ST7735_MADCTL);
    st7735_data(self, madctl);
    spi_disable_slave(&self->spi_device);
}

void st7735_set_background_color(st7735_t *self, st7735_color_t c)
{
    self->background_color = c;
}

void st7735_set_offset(st7735_t *self, int offset_x, int offset_y)
{
    self->offset_x = offset_x;
    self->offset_y = offset_y;
}

void st7735_clear_screen(st7735_t *self)
{
    st7735_fill_rectangle(self, 0, 0, self->width, self->height, self->background_color);
}

void st7735_draw_pixel(st7735_t *self, int x, int y, st7735_color_t c)
{
    st7735_fill_rectangle(self, x, y, 1, 1, c);
}

void st7735_fill_rectangle(st7735_t *self, int x, int y, int w, int h, st7735_color_t c)
{
    spi_enable_slave(&self->spi_device);

    st7735_set_draw_window(self, x, y, x+w-1, y+h-1);

    for(int x = 0; x < w*h; x++)
    {
        st7735_color(self, c);
    }

    spi_disable_slave(&self->spi_device);
}

void st7735_draw_char(st7735_t *self, int x, int y, const char chr, st7735_color_t color, int scale)
{
    if ( ! BETWEEN(chr, 0x20, 0x7E) ) return;

    int w = scale * ST7735_CHARSET_WIDTH;
    int h = scale * ST7735_CHARSET_HEIGHT;

    spi_enable_slave(&self->spi_device);
    st7735_set_draw_window(self, x, y, x+w-1, y+h-1);

    for (int dy = 0 ; dy < ST7735_CHARSET_HEIGHT ; dy++)
    {
        for (int i = 0 ; i < scale ; i++)
        {
            for (int dx = 0 ; dx < ST7735_CHARSET_WIDTH ; dx++)
            {
                uint8_t b = s_st7735_charset[chr-0x20][dx];
                st7735_color_t px = IS_SET_BIT(b, dy) ? color : self->background_color;

                for (int j = 0 ; j < scale ; j++)
                {
                    st7735_color(self, px);
                }
            }
        }
    }

    spi_disable_slave(&self->spi_device);
}

void st7735_draw_chars(st7735_t *self, int x, int y, const char* chars, int length, st7735_color_t color, int scale)
{
    for (int i = 0 ; i < length ; i++)
    {
        st7735_draw_char(self, x, y, chars[i], color, scale);
        x += scale * (ST7735_CHARSET_WIDTH + 1);
    }
}

void st7735_draw_string(st7735_t *self, int x, int y, const char* str, st7735_color_t color, int scale)
{
    for(int i = 0 ; str[i] != 0 ; i++)
    {
        st7735_draw_char(self, x, y, str[i], color, scale);
        x += scale * (ST7735_CHARSET_WIDTH + 1);
    }
}

void st7735_clear_char(st7735_t *self, int x, int y, int scale)
{
    int w = scale * ST7735_CHARSET_WIDTH;
    int h = scale * ST7735_CHARSET_HEIGHT;

    spi_enable_slave(&self->spi_device);
    st7735_set_draw_window(self, x, y, x+w-1, y+h-1);

    for (int dy = 0 ; dy < h ; dy++)
    {
        for (int dx = 0 ; dx < w ; dx++)
        {
            st7735_color(self, self->background_color);
        }
    }

    spi_disable_slave(&self->spi_device);
}

void st7735_clear_chars(st7735_t *self, int x, int y, int length, int scale)
{
    for (int i = 0 ; i < length ; i++)
    {
        st7735_clear_char(self, x, y, scale);
        x += scale * (ST7735_CHARSET_WIDTH + 1);
    }
}


// Std_ReturnType ST7735_DrawXPM(st7735_xpm_t *xpm, int xPos, int yPos, int scale)
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

static st7735_color_t ST7735_RenderXbm(int x, int y, int w, int h, void *data)
{
    UNUSED(h);
    UNUSED(w);

    XbmRendererData *d = TYPECAST(data, XbmRendererData*);
    uint8_t b = d->bits[y*d->bw + x/8];
    return IS_SET_BIT(b, x % 8) ? d->fg_color : d->bg_color;
}

void st7735_draw_xbm(st7735_t *self, st7735_xbm_t *bits, int x, int y, int w, int h, st7735_color_t c, int scale)
{
    XbmRendererData data = { bits, c, self->background_color, (w+7)/8 };
    st7735_render(self, x, y, w, h, ST7735_RenderXbm, &data, scale);
}

void st7735_render(st7735_t *self, int x, int y, int w, int h, ST7735_Renderer renderer, void* param, int scale)
{
    //Set the drawing region
    spi_enable_slave(&self->spi_device);
    st7735_set_draw_window(self, x, y, x+(scale*w)-1, y+(scale*h)-1);

    for(int y = 0; y < h; y++)
    {
        // Repeat each line according to scaling
        for(int _s1 = 0; _s1 < scale; _s1++)
        {
            for(int x = 0; x < w; x++)
            {
                st7735_color_t c = renderer(x, y, w, h, param);

                // Repeat each pixel according to scaling
                for(int _s2 = 0; _s2 < scale; _s2++)
                {
                    st7735_color(self, c);
                }
            }
        }
    }

    spi_disable_slave(&self->spi_device);
}

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
