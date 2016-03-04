#ifndef SRC_LCD_API_LCD_H_
#define SRC_LCD_API_LCD_H_

#include "types.h"

typedef enum
{
	DataLength_4bits,
	DataLength_8bits,
} DataLength;

typedef enum
{
	DisplayType_5x10,
	DisplayType_5x8,
} DisplayType;

Std_ReturnType LCD_Init();

Std_ReturnType LCD_FunctionSet(DataLength dl, DisplayType ft, uint8_t nbrOfLines);
Std_ReturnType LCD_SetCursor(uint8_t line, uint8_t pos);

#endif /* SRC_LCD_API_LCD_H_ */
