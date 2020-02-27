#include "avr/pgmspace.h"
#include "st7735.h"
#include "st7735_prv.h"
#include "st7735_cfg.h"
#include "spi.h"
#include "port.h"
#include "bits.h"
#include "charset.h"

void ST7735_Init()
{
    // Init GPIO's
    Port_SetPinDataDirection(ST7735_Pin_DC, Output);
    Port_SetPinState(ST7735_Pin_DC, High);

    // TFT startup routine
    ST7735_Command(ST7735_SWRESET);
    Os_Wait(120);

    ST7735_Command(ST7735_SLPOUT);
    Os_Wait(120);

    // Screen orientation
    ST7735_Command(ST7735_MADCTL);
    ST7735_Data(ST7735_SCREEN_ORIENTATION);

    // 16bits per pixel
    ST7735_Command(ST7735_COLMOD);
    ST7735_Data(0x05);

    ST7735_Command(ST7735_DISPON);
}

void ST7735_DrawXBM(const uint8_t *bits, const uint8_t xPos, const uint8_t yPos, const uint8_t width, const uint8_t height, const uint16_t foregroundColor, const uint16_t backgroundColor)
{
    ST7735_SetDrawWindow(xPos, yPos, xPos+width-1, yPos+height-1);
    for(int row = 0 ; row < height ; row++)
    {
        for(int col = 0 ; col < width ; /* do nothing */ )
        {
            int num = (col+8 >= width) ? width-col : 8;
            for(int k = 0 ; k < num ; k++)
            {
                if (GET_BIT(pgm_read_byte(bits), k) != 0)
                {
                    ST7735_Data(HIGH_BYTE(foregroundColor));
                    ST7735_Data(LOW_BYTE(foregroundColor));
                }
                else
                {
                    ST7735_Data(HIGH_BYTE(backgroundColor));
                    ST7735_Data(LOW_BYTE(backgroundColor));
                }
                col++;
            }
            bits++;
        }
    }
}

void ST7735_DrawPixel(const uint8_t x, const uint8_t y, const uint16_t color)
{
    ST7735_FillRectangle(x, y, 1, 1, color);
}

void ST7735_DrawLine(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2, const uint16_t color)
{
    // Integer only implementation of Bresenham's algorythm
    uint16_t CurrentX = x1, CurrentY = y1;
    int8_t Xinc = 1, Yinc = 1;
    int16_t Dx = x2-x1;
    int16_t Dy = y2-y1;

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

    ST7735_DrawPixel(x1, y1, color);

    if (Dy <= Dx)
    {
        int16_t TwoDxAccumulatedError = 0;
        do
        {
            CurrentX += Xinc;
            TwoDxAccumulatedError += 2*Dy;
            if (TwoDxAccumulatedError > Dx)
            {
                CurrentY += Yinc;
                TwoDxAccumulatedError -= 2*Dx;
            }

            ST7735_DrawPixel(CurrentX, CurrentY, color);
        }
        while(CurrentX != x2);
    }
    else
    {
        int16_t TwoDyAccumulatedError = 0;
        do
        {
            CurrentY += Yinc;
            TwoDyAccumulatedError += 2*Dx;
            if (TwoDyAccumulatedError > Dy)
            {
                CurrentX += Xinc;
                TwoDyAccumulatedError -= 2*Dy;
            }

            ST7735_DrawPixel(CurrentX, CurrentY, color);
        }
        while(CurrentY != y2);
    }

    ST7735_DrawPixel(x2, y2, color);
}

void ST7735_DrawChar(const uint8_t x, const uint8_t y, const char chr, const uint16_t foregroundColor, const uint16_t backgroundColor)
{
    if (chr < 0x20 /* || chr > 0x7F */ ) return;

    ST7735_SetDrawWindow(x, y, x+ST7735_CHARSET_WIDTH-1, y+ST7735_CHARSET_HEIGHT-1);

    for (int dy = 0 ; dy < ST7735_CHARSET_HEIGHT ; dy++)
    {
        for (int dx = 0 ; dx < ST7735_CHARSET_WIDTH ; dx++)
        {
            uint8_t b = pgm_read_byte(&s_st7735_charset[chr-0x20][dx]);
            if (IS_SET_BIT(b, dy))
            {
                ST7735_Data(HIGH_BYTE(foregroundColor));
                ST7735_Data(LOW_BYTE(foregroundColor));
            }
            else
            {
                ST7735_Data(HIGH_BYTE(backgroundColor));
                ST7735_Data(LOW_BYTE(backgroundColor));
            }
        }
    }
}

void ST7735_CharTest(void)
{
    uint8_t line = 0;
    uint8_t chr = 0;

    for (int i = 0x20 ; i <= 0x7F ; i++)
    {
        ST7735_DrawChar(8*chr++, 10*line, i, ST7735_COLOR_RED, ST7735_COLOR_BLACK);

        if (chr % 16 == 0)
        {
            line++;
            chr = 0;
        }
    }
}

void ST7735_ClearChar(const uint8_t x, const uint8_t y, const uint16_t backgroundColor)
{
    ST7735_SetDrawWindow(x, y, x+ST7735_CHARSET_WIDTH-1, y+ST7735_CHARSET_HEIGHT-1);

    // 7 pixels height
    for (int dy = 0 ; dy < ST7735_CHARSET_HEIGHT ; dy++)
    {
        // 5 pixels width
        for (int dx = 0 ; dx < ST7735_CHARSET_WIDTH ; dx++)
        {
            ST7735_Data(HIGH_BYTE(backgroundColor));
            ST7735_Data(LOW_BYTE(backgroundColor));
        }
    }
}

void ST7735_DrawString(const uint8_t x, const uint8_t y, const char* str, const uint16_t foregroundColor, const uint16_t backgroundColor)
{
    uint8_t xOffset = x;
//     for (uint8_t i = 0 ; i < length ; i++)
//     {
//         ST7735_DrawChar(xOffset, y, str[i], foregroundColor, backgroundColor);
//         xOffset += ST7735_CHARSET_WIDTH + ST7735_CHAR_SPACING;
//     }
    while( *str != 0 )
    {
        ST7735_DrawChar(xOffset, y, READ_PU8(str++), foregroundColor, backgroundColor);
        xOffset += ST7735_CHARSET_WIDTH + ST7735_CHAR_SPACING;
    }
}

void ST7735_ClearString(const uint8_t x, const uint8_t y, const int length, const uint16_t backgroundColor)
{
    for (int xOffset = x, i = 0 ; i < length ; i++)
    {
        ST7735_ClearChar(xOffset, y, backgroundColor);
        xOffset += ST7735_CHARSET_WIDTH + ST7735_CHAR_SPACING;
    }
}

void ST7735_FillRectangle(const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h, const uint16_t color)
{
    //Set the drawing region
    ST7735_SetDrawWindow(x, y, x+w-1, y+h-1);

    //Write color to each pixel
    for(int x = 0; x < w*h; x++)
    {
        ST7735_Data(HIGH_BYTE(color));
        ST7735_Data(LOW_BYTE(color));
    }
}

void ST7735_SetDrawWindow(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2)
{
    // Set the column to write to
    ST7735_Command(ST7735_CASET);
    ST7735_Data(0);
    ST7735_Data(x1);
    ST7735_Data(0);
    ST7735_Data(x2);

    // Set the row range to write to
    ST7735_Command(ST7735_RASET);
    ST7735_Data(0);
    ST7735_Data(y1);
    ST7735_Data(0);
    ST7735_Data(y2);

    // Write to RAM
    ST7735_Command(ST7735_RAMWR);
}

void ST7735_Command(const uint8_t command)
{
    Port_SetPinState(ST7735_Pin_DC, Low);
    Spi_WriteByte(command, NULL_PTR);
    Port_SetPinState(ST7735_Pin_DC, High);
}

void ST7735_Data(const uint8_t data)
{
    Spi_WriteByte(data, NULL_PTR);
}
