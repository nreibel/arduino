#ifndef SRC_PORT_API_GPUI_UNO_H_
#define SRC_PORT_API_GPUI_UNO_H_

#include "port.h"

#define GPIO_A0 {Port_C, 0}
#define GPIO_A1 {Port_C, 1}
#define GPIO_A2 {Port_C, 2}
#define GPIO_A3 {Port_C, 3}
#define GPIO_A4 {Port_C, 4}
#define GPIO_A5 {Port_C, 5}

#define GPIO_13  {Port_B, 5}
#define GPIO_12  {Port_B, 4}
#define GPIO_11  {Port_B, 3}
#define GPIO_10  {Port_B, 2}
#define GPIO_9   {Port_B, 1}
#define GPIO_8   {Port_B, 0}
#define GPIO_7   {Port_D, 7}
#define GPIO_6   {Port_D, 6}
#define GPIO_5   {Port_D, 5}
#define GPIO_4   {Port_D, 4}
#define GPIO_3   {Port_D, 3}
#define GPIO_2   {Port_D, 2}
#define GPIO_1   {Port_D, 1}
#define GPIO_0   {Port_D, 0}

// Aliases
#define GPIO_RX GPIO_0
#define GPIO_TX GPIO_1

#define GPIO_SCK  GPIO_13
#define GPIO_MISO GPIO_12
#define GPIO_MOSI GPIO_11
#define GPIO_SS   GPIO_10

#define GPIO_INT0 GPIO_2
#define GPIO_INT1 GPIO_3

#endif // SRC_PORT_API_GPUI_UNO_H_
