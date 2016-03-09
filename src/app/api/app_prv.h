#ifndef SRC_APP_API_APP_PRV_H_
#define SRC_APP_API_APP_PRV_H_

#include "port.h"
#include "app_cfg.h"

#if BUZZER_ENABLED == ON
typedef enum
{
	Continuous,
	Beep,
	Off
} BuzzerState;
#endif

/* Output of status LED */
extern PinDef Pin_LED;

/* Enable sensor and accessories */
extern PinDef Pin_POWER;

void Task_MainCyclic(void);
void Task_Blink(void);

#if BUZZER_ENABLED == ON
void Task_Buzzer(void);
#endif

#if LCD_ENABLED == ON
void LCD_CyclicTask(void);
#endif

#endif /* SRC_APP_API_APP_PRV_H_ */
