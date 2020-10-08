#include "st7735.h"
#include "st7735_prv.h"
#include "st7735_cfg.h"
#include "os.h"
#include "spi.h"
#include "gpio.h"
#include "bits.h"
#include "charset.h"
#include "stdio.h"

static st7735_color_t background_color = ST7735_COLOR_BLACK;

void ST7735_Init()
{
    // Init GPIO's
    GPIO_SetDataDirection(ST7735_Pin_DC, GPIO_Output);
    GPIO_SetState(ST7735_Pin_DC, GPIO_High);

    // TFT startup routine
    ST7735_Command(ST7735_SWRESET);
    Os_Wait(120);

    ST7735_Command(ST7735_SLPOUT);
    Os_Wait(120);

    // 16bits per pixel
    ST7735_Command(ST7735_COLMOD);
    ST7735_Data(ST7735_COLMOD_16_BPP);

    // Screen orientation
    uint8_t madctl = 0;

#ifndef ST7735_SCREEN_ORIENTATION
#error ST7735_SCREEN_ORIENTATION is not defined
#elif ST7735_SCREEN_ORIENTATION == ST7735_SCREEN_ORIENTATION_PORTRAIT
    madctl = 0;
#elif ST7735_SCREEN_ORIENTATION == ST7735_SCREEN_ORIENTATION_LANDSCAPE
    madctl = ST7735_MADCTL_MV | ST7735_MADCTL_MY;
#elif ST7735_SCREEN_ORIENTATION == ST7735_SCREEN_ORIENTATION_PORTRAIT_INV
    madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MX;
#elif ST7735_SCREEN_ORIENTATION == ST7735_SCREEN_ORIENTATION_LANDSCAPE_INV
    madctl = ST7735_MADCTL_MV | ST7735_MADCTL_MX;
#else
#error ST7735_SCREEN_ORIENTATION is not valid
#endif

    ST7735_Command(ST7735_MADCTL);
    ST7735_Data(madctl);

    ST7735_Command(ST7735_DISPON);
}

void ST7735_SetBackgroundColor(st7735_color_t c)
{
    background_color = c;
}

Std_ReturnType ST7735_DrawXPM(st7735_xpm_t *xpm, int xPos, int yPos, int scale)
{
    int width, height, nbColors, sz;
    st7735_color_t colors[16];

    // Read image attributes
    if ( sscanf(xpm[0], "%d %d %d %d", &width, &height, &nbColors, &sz) != 4 ) return Status_Not_OK;

    // Can't handle more than 1 byte color indices
    if (sz != 1) return Status_Not_OK;

    // Can't handle more than 16 colors
    if (nbColors > 16) return Status_Not_OK;

    // Read all colors
    for (int i = 0 ; i < nbColors ; i++)
    {
        char name, chr;
        int r, g, b;

        if ( sscanf(xpm[1+i], "%c c #%2x%2x%2x", &name, &r, &g, &b) == 4 )
        {
            // Actual color
            colors[i] = ST7735_RED(r) | ST7735_GREEN(g) | ST7735_BLUE(b);
        }
        else if ( sscanf(xpm[1+i], "%c c Non%c", &name, &chr) == 2 && chr == 'e')
        {
            // Transparent color
            colors[i] = background_color;
        }
        else return Status_Not_OK;

        // Replace in image data all names with color index
        for (int x = 0 ; x < height ; x++)
        {
            for (int y = 0 ; y < width ; y++)
            {
                if (xpm[1 + nbColors + x][y] == name)
                {
                    xpm[1 + nbColors + x][y] = i;
                }
            }
        }
    }

    // Draw the image
    ST7735_SetDrawWindow(xPos, yPos, xPos+(width*scale)-1, yPos+(height*scale)-1);
    for (int x = 0 ; x < height ; x++)
    {
        // Repeat each line with scale factor
        for (int _s1 = 0 ; _s1 < scale ; _s1++)
        {
            for (int y = 0 ; y < width ; y++)
            {
                // Repeat each pixel with scale factor
                int idx = xpm[1 + nbColors + x][y];
                for (int _s2 = 0 ; _s2 < scale ; _s2++)
                {
                    ST7735_Color( colors[idx] );
                }
            }
        }
    }

    return Status_OK;
}

static st7735_color_t ST7735_RenderXbm(int x, int y, int w, int h, void *data)
{
    UNUSED(h);
    UNUSED(w);

    XbmRendererData *d = TYPECAST(data, XbmRendererData*);
    uint8_t b = d->bits[y*d->bw + x/8];
    return IS_SET_BIT(b, x % 8) ? d->color : background_color;
}

void ST7735_DrawXBM(st7735_xbm_t *bits, int x, int y, int w, int h, st7735_color_t c, int scale)
{
    XbmRendererData data = { bits, c, (w+7)/8 };
    ST7735_Render(x, y, w, h, ST7735_RenderXbm, &data, scale);
}

void ST7735_ClearScreen()
{
    ST7735_FillRectangle(0, 0, ST7735_SCREEN_WIDTH, ST7735_SCREEN_HEIGHT, background_color);
}

void ST7735_DrawPixel(int x, int y, st7735_color_t c)
{
    ST7735_FillRectangle(x, y, 1, 1, c);
}

void ST7735_Render(int x, int y, int w, int h, ST7735_Renderer renderer, void* param, int scale)
{
    //Set the drawing region
    ST7735_SetDrawWindow(x, y, x+(scale*w)-1, y+(scale*h)-1);

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
                    ST7735_Color(c);
                }
            }
        }
    }
}

void ST7735_DrawLine(int x1, int y1, int x2, int y2, st7735_color_t c)
{
    // Integer only implementation of Bresenham's algorythm
    int CurrentX = x1, CurrentY = y1;
    int Xinc = 1, Yinc = 1;
    int Dx = x2-x1;
    int Dy = y2-y1;

    if (Dx < 0)
    {
        Xinc = -1;
        Dx = -Dx;
    }

    if (Dy < 0)
    {
        Yinc = -1;
        Dy = -Dy;
    }

    ST7735_DrawPixel(x1, y1, c);

    if (Dy <= Dx)
    {
        int TwoDxAccumulatedError = 0;
        do
        {
            CurrentX += Xinc;
            TwoDxAccumulatedError += 2*Dy;
            if (TwoDxAccumulatedError > Dx)
            {
                CurrentY += Yinc;
                TwoDxAccumulatedError -= 2*Dx;
            }

            ST7735_DrawPixel(CurrentX, CurrentY, c);
        }
        while(CurrentX != x2);
    }
    else
    {
        int TwoDyAccumulatedError = 0;
        do
        {
            CurrentY += Yinc;
            TwoDyAccumulatedError += 2*Dx;
            if (TwoDyAccumulatedError > Dy)
            {
                CurrentX += Xinc;
                TwoDyAccumulatedError -= 2*Dy;
            }

            ST7735_DrawPixel(CurrentX, CurrentY, c);
        }
        while(CurrentY != y2);
    }

    ST7735_DrawPixel(x2, y2, c);
}

void ST7735_CharTest(void)
{
    int line = 0;
    int chr = 0;

    for (int i = 0x20 ; i <= 0x7F ; i++)
    {
        ST7735_DrawChar(8*chr++, 10*line, i, ST7735_COLOR_RED);

        if (chr % 16 == 0)
        {
            line++;
            chr = 0;
        }
    }
}

void ST7735_DrawChar(int x, int y, char chr, st7735_color_t c)
{
    if ( ! BETWEEN(chr, 0x20, 0x7E) ) return;

    ST7735_SetDrawWindow(x, y, x+ST7735_CHARSET_WIDTH-1, y+ST7735_CHARSET_HEIGHT-1);

    for (int dy = 0 ; dy < ST7735_CHARSET_HEIGHT ; dy++)
    {
        for (int dx = 0 ; dx < ST7735_CHARSET_WIDTH ; dx++)
        {
            uint8_t b = s_st7735_charset[chr-0x20][dx];
            ST7735_Color( IS_SET_BIT(b, dy) ? c : background_color );
        }
    }
}

void ST7735_DrawChars(int x, int y, buffer_t chars, int length, st7735_color_t c)
{
    for (int i = 0 ; i < length ; i++)
    {
        ST7735_DrawChar(x, y, TYPECAST(chars, char*)[i], c);
        x += ST7735_CHARSET_WIDTH + ST7735_CHAR_SPACING;
    }
}

void ST7735_ClearChar(int x, int y)
{
    ST7735_SetDrawWindow(x, y, x+ST7735_CHARSET_WIDTH-1, y+ST7735_CHARSET_HEIGHT-1);

    // 7 pixels height
    for (int dy = 0 ; dy < ST7735_CHARSET_HEIGHT ; dy++)
    {
        // 5 pixels width
        for (int dx = 0 ; dx < ST7735_CHARSET_WIDTH ; dx++)
        {
            ST7735_Color(background_color);
        }
    }
}

void ST7735_ClearChars(int x, int y, int length)
{
    for (int i = 0 ; i < length ; i++)
    {
        ST7735_ClearChar(x, y);
        x += ST7735_CHARSET_WIDTH + ST7735_CHAR_SPACING;
    }
}

void ST7735_DrawString(int x, int y, char* str, st7735_color_t c)
{
    while(*str != 0)
    {
        ST7735_DrawChar(x, y, READ_PU8(str++), c);
        x += ST7735_CHARSET_WIDTH + ST7735_CHAR_SPACING;
    }
}

void ST7735_FillRectangle(int x, int y, int w, int h, st7735_color_t c)
{
    //Set the drawing region
    ST7735_SetDrawWindow(x, y, x+w-1, y+h-1);

    //Write color to each pixel
    for(int x = 0; x < w*h; x++)
    {
        ST7735_Color(c);
    }
}

static void ST7735_SetDrawWindow(int x1, int y1, int x2, int y2)
{
    // Set the column to write to
    ST7735_Command(ST7735_CASET);
    ST7735_Data(0);
    ST7735_Data(x1 + ST7735_OFFSET_X);
    ST7735_Data(0);
    ST7735_Data(x2 + ST7735_OFFSET_X);

    // Set the row range to write to
    ST7735_Command(ST7735_RASET);
    ST7735_Data(0);
    ST7735_Data(y1 + ST7735_OFFSET_Y);
    ST7735_Data(0);
    ST7735_Data(y2 + ST7735_OFFSET_Y);

    // Write to RAM
    ST7735_Command(ST7735_RAMWR);
}

static void ST7735_Command(uint8_t command)
{
    GPIO_SetState(ST7735_Pin_DC, GPIO_Low);
    Spi_WriteByte(command, NULL_PTR);
    GPIO_SetState(ST7735_Pin_DC, GPIO_High);
}

static void ST7735_Data(uint8_t data)
{
    Spi_WriteByte(data, NULL_PTR);
}

static void ST7735_Color(st7735_color_t color)
{
    word w = {color};
    Spi_WriteByte(w.byte[1], NULL_PTR);
    Spi_WriteByte(w.byte[0], NULL_PTR);
}
