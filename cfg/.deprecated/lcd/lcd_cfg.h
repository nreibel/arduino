#ifndef SRC_LCD_CFG_LCD_CFG_H_
#define SRC_LCD_CFG_LCD_CFG_H_

#include "port.h"

Port_GPIO pin_RS = {Port_D, Pin_4};
Port_GPIO pin_EN = {Port_D, Pin_5};

Port Port_Data = Port_B;

#endif /* SRC_LCD_CFG_LCD_CFG_H_ */
