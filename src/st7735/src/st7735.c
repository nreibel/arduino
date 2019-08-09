#include "st7735.h"
#include "st7735_prv.h"
#include "st7735_cfg.h"
#include "spi.h"
#include "port.h"
#include "bits.h"

static const uint8_t s_st7735_charset[][ST7735_CHARSET_WIDTH] = {
    {0x08, 0x1C, 0x36, 0x1C, 0x08}, // Unknown character
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    {0x04, 0x08, 0x10, 0x08, 0x04}, // v
    {0x10, 0x08, 0x04, 0x08, 0x10}, // ^
    {0x41, 0x22, 0x14, 0x08, 0x00}, // >
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x1C, 0x22, 0x41, 0x41, 0x41}, // C
    {0x7F, 0x41, 0x41, 0x41, 0x3E}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x49}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x41, 0x51, 0x32}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x61, 0x3E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x26, 0x49, 0x49, 0x49, 0x32}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x0F, 0x30, 0x40, 0x30, 0x0F}, // V
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
    {0x41, 0x51, 0x49, 0x45, 0x41}, // Z
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x02, 0x7F, 0x00, 0x00}, // 1
    {0x62, 0x51, 0x49, 0x49, 0x46}, // 2
    {0x22, 0x49, 0x49, 0x49, 0x36}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3E, 0x49, 0x49, 0x49, 0x32}, // 6
    {0x01, 0x01, 0x79, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x26, 0x49, 0x49, 0x49, 0x3E}, // 9
};

void ST7735_Init()
{
    // Init GPIO's
    Port_SetPinDataDirection(ST7735_Pin_DC, Output);
    Port_SetPinState(ST7735_Pin_DC, High);

    Port_SetPinDataDirection(ST7735_Pin_RS, Output);
    Port_SetPinState(ST7735_Pin_RS, High);

    /*
    Port_SetPinState(ST7735_Pin_RS, Low);
    Os_Wait(500);
    Port_SetPinState(ST7735_Pin_RS, High);
    Os_Wait(500);
    */
    
    // TFT startup routine
    ST7735_Command(ST7735_SWRESET);
    Os_Wait(120);
    
    ST7735_Command(ST7735_SLPOUT);
    Os_Wait(120);
    
    ST7735_Command(ST7735_MADCTL); // Screen orientation
    ST7735_Data(ST7735_SCREEN_ORIENTATION);
    
    ST7735_Command(ST7735_COLMOD);
    ST7735_Data(0x05); // 16bits per pixel
    
    ST7735_Command(ST7735_DISPON);
}

void ST7735_DrawPixel(const uint8_t x, const uint8_t y, const uint16_t color)
{
    ST7735_FillRectangle(x, y, x, y, color);
}

void ST7735_DrawChar(const uint8_t x, const uint8_t y, const uint8_t chr, const uint16_t foregroundColor, const uint16_t backgroundColor)
{
    int c = 0; // Unknown character by default
    
    if ( chr == ' ' )
    {
        c = 1;
    }
    else if (chr == 'v')
    {
        c = 2;
    }
    else if (chr == '^')
    {
        c = 3;
    }
    else if (chr == '>')
    {
        c = 4;
    }
    else if (chr == '<')
    {
        c = 5;
    }
    else if (chr >= 'A' && chr <= 'Z')
    {
        c = chr - 'A' + 6;
    }
    else if (chr >= '0' && chr <= '9')
    {
        c = chr - '0' + 32;
    }
    
    ST7735_SetDrawWindow(x, y, x+ST7735_CHARSET_WIDTH-1, y+ST7735_CHARSET_HEIGHT-1);

    // 7 pixels height
    for (int dy = 0 ; dy < ST7735_CHARSET_HEIGHT ; dy++)
    {
        // 5 pixels width
        for (int dx = 0 ; dx < ST7735_CHARSET_WIDTH ; dx++)
        {
            if (IS_SET_BIT(s_st7735_charset[c][dx], dy))
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

void ST7735_DrawString(const uint8_t x, const uint8_t y, const uint8_t *str, int length, const uint16_t foregroundColor, const uint16_t backgroundColor)
{
    int offset = x;
    for (int i = 0 ; i < length ; i++)
    {
        ST7735_DrawChar(offset, y, str[i], foregroundColor, backgroundColor);
        offset += CHAR_SPACING;
    }
}

void ST7735_FillRectangle(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2, const uint16_t color)
{
    //Set the drawing region
    ST7735_SetDrawWindow(x1, y1, x2, y2);

    //Write color to each pixel
    for(int x = 0; x < (x2-x1+1)*(y2-y1+1); x++)
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
    Spi_FastWrite(command);
    Port_SetPinState(ST7735_Pin_DC, High);
}

void ST7735_Data(const uint8_t data)
{
    Spi_FastWrite(data);
}
