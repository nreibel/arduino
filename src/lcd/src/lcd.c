#include "timer.h"
#include "lcd.h"
#include "lcd_cfg.h"
#include "lcd_prv.h"
#include "port.h"
#include "bits.h"
#include "os.h"
#include "avr/io.h"

static bool requestRefresh = FALSE;
static LcdStatus lcdState = Uninitialized;
static char buffer[33] = {0};

void LCD_RequestRefresh()
{
    requestRefresh = TRUE;
}

Std_ReturnType LCD_GetBuffer(char** buffer_ptr)
{
    Std_ReturnType retval = Status_Not_OK;

    if (lcdState == Ready && requestRefresh == FALSE)
    {
        *buffer_ptr = buffer;
        retval = Status_OK;
    }
    else if (lcdState == Busy)
    {
        retval = Status_Pending;
    }

    if (retval != Status_OK)
    {
        *buffer_ptr = NULL_PTR;
    }

    return retval;
}

/*
 * The cyclic task periodically checks if a screen refresh is requested.
 * Actual printing is done by the background task.
 * This function dictates the refresh rate of the screen.
 */
void LCD_CyclicTask()
{
    if (lcdState == Ready && requestRefresh == TRUE)
    {
        LCD_ClearDisplay();
        LCD_ReturnHome();
        requestRefresh = FALSE;

        /* Signals the background task to start writing */
        lcdState = Busy;
    }
}

/*
 * The background task is in charge of writing data to the screen
 */
void LCD_BackgroundTask()
{
    static char* ptr = buffer;

    if ( lcdState == Busy )
    {

        if ( *ptr == 0 || ptr == buffer + 32 )
        {
            /* Cleanup and prepare for next refresh */
            ptr = buffer;
            lcdState = Ready;
        }
        else
        {
            /* Jump to next line */
            if (ptr == buffer + 16)
            {
                LCD_SetCursor(2, 0);
            }

            /* Write the next character */
            LCD_Write_Char( *ptr );
            ptr++;
        }
    }
}

Std_ReturnType LCD_Init()
{
    Std_ReturnType retval = Status_Not_OK;

    uint8_t port_ddr = 0x0;

    if ( Port_GetDataDirection(Port_Data, &port_ddr) == Status_OK )
    {
        SET_MASK(port_ddr, 0xF);
        Port_SetDataDirection(Port_Data, port_ddr);

        Port_SetPinDataDirection(pin_EN, Output);
        Port_SetPinDataDirection(pin_RS, Output);

        retval = Status_OK;
    }

    /* Give time for hardware to init*/
    Os_Wait(15);

    /* First write sets data width*/
    LCD_WriteNibble(0x2, Low);
    Os_Wait(4);

    LCD_FunctionSet(DataWidth_4_Bits, CharacterFont_5x8, DisplayLines_2_Line);
    LCD_DisplayControl(TRUE, FALSE, FALSE);
    LCD_SetEntryMode(Increment, FALSE);
    LCD_ClearDisplay();
    LCD_ReturnHome();

    lcdState = Ready;

    return retval;
}

Std_ReturnType LCD_Write_Char(uint8_t chr)
{
    Std_ReturnType retval = Status_OK;

    if (chr < 0x20 || chr > 0x7F)
    {
        retval = Status_Not_OK;
    }

    if (retval == Status_OK)
    {
        retval = LCD_WriteByte(chr, High);
    }

    return retval;
}

Std_ReturnType LCD_ClearDisplay()
{
    Std_ReturnType retval = Status_OK;

    uint8_t data = 0x01;

    if (retval == Status_OK)
    {
        retval = LCD_WriteByte(data, Low);
    }

    return retval;
}

Std_ReturnType LCD_ReturnHome()
{
    Std_ReturnType retval = Status_OK;

    uint8_t data = 0x02;

    if (retval == Status_OK)
    {
        retval = LCD_WriteByte(data, Low);
    }

    return retval;
}

Std_ReturnType LCD_SetEntryMode(CursorMoveDirection direction, bool automaticDisplayShift)
{
    Std_ReturnType retval = Status_OK;

    uint8_t data = 0x04;

    switch(direction)
    {
        case Decrement: RESET_BIT(data, 1); break;
        case Increment:   SET_BIT(data, 1); break;
        default: retval = Status_Not_OK; /* Invalid data */
    }

    if (automaticDisplayShift == TRUE) SET_BIT(data, 0);

    if (retval == Status_OK)
    {
        retval = LCD_WriteByte(data, Low);
    }

    return retval;
}

Std_ReturnType LCD_DisplayControl(bool displayOn, bool cursonOn, bool cursorBlinking)
{
    Std_ReturnType retval = Status_OK;

    uint8_t data = 0x08;

    if (displayOn      == TRUE) SET_BIT(data, 2);
    if (cursonOn       == TRUE) SET_BIT(data, 1);
    if (cursorBlinking == TRUE) SET_BIT(data, 0);

    if (retval == Status_OK)
    {
        retval = LCD_WriteByte(data, Low);
    }

    return retval;
}

Std_ReturnType LCD_SetCursor(uint8_t line, uint8_t pos)
{
    Std_ReturnType retval = Status_OK;

    uint8_t data = 0x80;

    switch(line)
    {
        case 1: RESET_BIT(data, 6); break;
        case 2:   SET_BIT(data, 6); break;
        default: retval = Status_Not_OK; /* Invalid data */
    }

    if (pos <= 0xF)
    {
        SET_MASK(data, pos);
    }
    else
    {
        retval = Status_Not_OK;
    }

    if (retval == Status_OK)
    {
        retval = LCD_WriteByte(data, Low);
    }

    return retval;
}

Std_ReturnType LCD_FunctionSet(InterfaceDataWidth dataWidth, CharacterFont characterFont, DisplayLines displayLines)
{
    Std_ReturnType retval = Status_OK;

    uint8_t data = 0x20;

    switch(dataWidth)
    {
        case DataWidth_4_Bits: RESET_BIT(data, 4); break;
        case DataWidth_8_Bits:   SET_BIT(data, 4); break;
        default: retval = Status_Not_OK; /* Invalid data */
    }

    switch(displayLines)
    {
        case DisplayLines_1_Line: RESET_BIT(data, 3); break;
        case DisplayLines_2_Line:   SET_BIT(data, 3); break;
        default: retval = Status_Not_OK; /* Invalid data */
    }

    switch(characterFont)
    {
        case CharacterFont_5x8: RESET_BIT(data, 2); break;
        case CharacterFont_5x10:  SET_BIT(data, 2); break;
        default: retval = Status_Not_OK; /* Invalid data */
    }

    if (retval == Status_OK)
    {
        retval = LCD_WriteByte(data, Low);
    }

    return retval;
}

Std_ReturnType LCD_WriteByte(uint8_t data, PinState rs)
{
    Std_ReturnType retval = Status_Not_OK;
    Std_ReturnType retval_high_nibble = Status_Not_OK;
    Std_ReturnType retval_low_nibble = Status_Not_OK;

    // Write high nibble
    retval_high_nibble = LCD_WriteNibble( HIGH_NIBBLE(data), rs );

    // Write low nibble
    retval_low_nibble = LCD_WriteNibble( LOW_NIBBLE(data), rs );

    if (retval_high_nibble == Status_OK && retval_low_nibble == Status_OK)
    {
        retval = Status_OK;
    }

    return retval;
}

Std_ReturnType LCD_WriteNibble(uint8_t data, PinState rs)
{
    Std_ReturnType retval = Status_Not_OK;

    uint8_t portVal;

    Port_SetPinState(pin_EN, High);

    if (Status_OK == Port_GetValue(Port_Data, &portVal))
    {
        RESET_MASK(portVal, 0xF);
        SET_MASK(portVal, data & 0xF);
        Port_SetValue(Port_Data, portVal);
        retval = Status_OK;
    }

    Port_SetPinState(pin_RS, rs);

    Os_Wait(1);
    Port_SetPinState(pin_EN, Low);
    Os_Wait(1);

    return retval;
}
