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
#include "stack.h"
#include "serial.h"
#include "math.h"
#include "stdio.h"

#if LCD_ENABLED == ON
#include "lcd.h"
#endif

#if BUZZER_ENABLED == ON
#include "pwm.h"
static BuzzerState buzzerState = Off;
#endif

#if SERIAL_DEBUG_ENABLED == ON
static char serial_buffer[10];
#endif

void App_Init()
{
	USS_Init();

#if LCD_ENABLED == ON
	LCD_Init();
#endif

#if SERIAL_DEBUG_ENABLED == ON
	Serial_Init();
#endif

#if BUZZER_ENABLED == ON
	PWM_Init();
#endif

	/* Set LED pin as OUTPUT */
	Port_SetPinDataDirection(App_Cfg_Port_LED, App_Cfg_Pin_LED, Output);

	/* Set up tasks */
	Timer_InitTask(Timer_BlinkTask, 500, &Task_Blink);
	Timer_InitTask(Timer_MainTask,  100, &Task_MainCyclic);

	/* Start all tasks */
	Timer_Enable(Timer_BlinkTask);
	Timer_Enable(Timer_MainTask);

#if BUZZER_ENABLED == ON
	Timer_InitTask(Timer_BuzzTask,  100, &Task_Buzzer);
	Timer_Enable(Timer_BuzzTask);
#endif

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

#if BUZZER_ENABLED == ON
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
#endif

#if SERIAL_DEBUG_ENABLED == ON
		if ( Serial_IsReady() == Status_OK )
		{
			sprintf(serial_buffer, "%d cm\n\r", average);
			Serial_Print(serial_buffer);
		}
#endif

		/* Trigger next acquisition */
		USS_TriggerMeasurement();
	}
}

/* Controls the buzzer */

#if BUZZER_ENABLED == ON
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
#endif
