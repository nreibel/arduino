#ifndef SRC_LCD_API_LCD_PRV_H_
#define SRC_LCD_API_LCD_PRV_H_

extern PinDef pin_RS;
extern PinDef pin_EN;

Std_ReturnType Write_Byte(uint8_t data, PinState rs);
Std_ReturnType Write_Half_Byte(uint8_t data, PinState rs);

#endif /* SRC_LCD_API_LCD_PRV_H_ */
