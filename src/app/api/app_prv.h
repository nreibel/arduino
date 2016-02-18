#ifndef SRC_APP_API_APP_PRV_H_
#define SRC_APP_API_APP_PRV_H_

typedef enum
{
	Continuous,
	Beep,
	Off
} BuzzerState;

void Task_Blink(void);
void Task_Buzzer(void);
void Task_MainCyclic(void);

#endif /* SRC_APP_API_APP_PRV_H_ */
