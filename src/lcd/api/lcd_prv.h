#ifndef SRC_LCD_API_LCD_PRV_H_
#define SRC_LCD_API_LCD_PRV_H_

extern PinDef pin_RS;
extern PinDef pin_EN;

typedef enum {
	Uninitialized,
	Ready,
	Busy
} LcdStatus;

void LCD_CyclicTask();
Std_ReturnType LCD_Write_Char(uint8_t chr);
Std_ReturnType LCD_WriteByte(uint8_t data, PinState rs);
Std_ReturnType LCD_WriteNibble(uint8_t data, PinState rs);

#endif /* SRC_LCD_API_LCD_PRV_H_ */
