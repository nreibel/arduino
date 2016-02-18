#include "os.h"
#include "app_prv.h"
#include "app_cfg.h"
#include "timer.h"
#include "timer_cfg.h"
#include "port.h"
#include "uss.h"
#include "stdio.h"
#include "avr/io.h"
#include "bits.h"
#include "pwm.h"
#include "stack.h"
#include "math.h"

static BuzzerState buzzerState = Off;

void App_Init()
{
	USS_Init();
	PWM_Init();

	/* Set LED pin as OUTPUT */
	Port_SetPinDataDirection(App_Cfg_Port_LED, App_Cfg_Pin_LED, Output);

	/* Set up tasks */
	Timer_InitTask(Timer_BlinkTask, 500, &Task_Blink);
	Timer_InitTask(Timer_MainTask,  100, &Task_MainCyclic);
	Timer_InitTask(Timer_BuzzTask,  100, &Task_Buzzer);

	/* Start all tasks */
	Timer_Enable(Timer_BlinkTask);
	Timer_Enable(Timer_MainTask);
	Timer_Enable(Timer_BuzzTask);

	USS_TriggerMeasurement();
}

/* Blinks a LED to indicate status */
void Task_Blink()
{
	static PinState pinState = Low;

	pinState = (pinState == Low ? High : Low);
	Port_SetPinState(App_Cfg_Port_LED, App_Cfg_Pin_LED, pinState);
}

/* Main task : read USS sensor and set buzzing mode */
void Task_MainCyclic(void)
{
	static Stack *stack = NULL_PTR;
	static uint16_t distance;
	static uint16_t average;

	if (stack == NULL_PTR)
	{
		stack = Stack_Create(STACK_SIZE);
		Stack_Init(stack, 500);
	}

	if ( USS_GetDistance(&distance) == Status_OK )
	{
		Stack_Push(stack, distance);

		average = Math_Average_u16(stack->buffer, stack->size);
		if (average < 15)
		{
			buzzerState = Continuous;
		}
		else if (average < 100)
		{
			int interval = 10 * (average - 10);
			Timer_SetTriggerTime(Timer_BuzzTask, interval);
			buzzerState = Beep;
		}
		else
		{
			buzzerState = Off;
		}

		/* Trigger next acquisition */
		USS_TriggerMeasurement();
	}
}

/* Controls the buzzer */

void Task_Buzzer(void)
{
	static boolean isOn = FALSE;

	if ((buzzerState == Continuous || buzzerState == Beep) &&  isOn == FALSE)
	{
		PWM_SetPWM(127);
		isOn = TRUE;
	}
	else if ((buzzerState == Off || buzzerState == Beep) && isOn == TRUE)
	{
		PWM_StopPWM();
		isOn = FALSE;
	}
}
