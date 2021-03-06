#ifndef SRC_LCD_API_LCD_H_
#define SRC_LCD_API_LCD_H_

#include "types.h"

typedef enum
{
    DataWidth_4_Bits,
    DataWidth_8_Bits,
} InterfaceDataWidth;

typedef enum
{
    CharacterFont_5x10,
    CharacterFont_5x8,
} CharacterFont;

typedef enum
{
    DisplayLines_1_Line,
    DisplayLines_2_Line,
} DisplayLines;

typedef enum
{
    Increment,
    Decrement,
} CursorMoveDirection;

Std_ReturnType LCD_Init();
Std_ReturnType LCD_ClearDisplay();
Std_ReturnType LCD_ReturnHome();
Std_ReturnType LCD_SetEntryMode(CursorMoveDirection direction, bool autoamticDisplayShift);
Std_ReturnType LCD_DisplayControl(bool displayOn, bool cursonOn, bool cursorBlinking);
Std_ReturnType LCD_FunctionSet(InterfaceDataWidth dataWidth, CharacterFont characterFont, DisplayLines displayLines);
Std_ReturnType LCD_SetCursor(uint8_t line, uint8_t pos);

Std_ReturnType LCD_GetBuffer(char** buffer_ptr);
void LCD_RequestRefresh();
void LCD_BackgroundTask();

#endif /* SRC_LCD_API_LCD_H_ */
