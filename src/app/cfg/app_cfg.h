#ifndef SRC_APP_CFG_APP_CFG_H_
#define SRC_APP_CFG_APP_CFG_H_

#include "types.h"
#include "port.h"

#define STACK_SIZE 4


#define SERIAL_DEBUG_ENABLED OFF
#define BUZZER_ENABLED       OFF
#define LCD_ENABLED          ON

/* Output of status LED */
extern Port App_Cfg_Port_LED;
extern Pin  App_Cfg_Pin_LED;

#endif /* SRC_APP_CFG_APP_CFG_H_ */
