#include "os.h"
#include "app.h"
#include "app_prv.h"
#include "app_cfg.h"
#include "timer.h"
#include "port.h"
#include "uss.h"
#include "stdio.h"
#include "bits.h"
#include "stack.h"
#include "math.h"

#if LCD_ENABLED == ON
#include "lcd.h"
static boolean requestRefresh = FALSE;
static uint16_t lcdLastValue = 0;
static char lcd_buffer[10];
#endif

#if BUZZER_ENABLED == ON
#include "pwm.h"
static BuzzerState buzzerState = Off;
#endif

#if SERIAL_DEBUG_ENABLED == ON
#include "serial.h"
static char serial_buffer[10];
#endif

void App_Init()
{
	/* Power on accessories/sensors */
	Port_SetPinDataDirection(Pin_POWER, Output);
	Port_SetPinState(Pin_POWER, High);

	/* Init ultrasound */
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

	Port_SetPinDataDirection(Pin_LED, Output);

	/* Set up tasks */
	Timer_InitTask(Timer_BlinkTask,   500,  &Task_Blink);
	Timer_InitTask(Timer_MainTask,    100,  &Task_MainCyclic);

#if BUZZER_ENABLED == ON
	Timer_InitTask(Timer_BuzzTask,  100, &Task_Buzzer);
#endif

#if LCD_ENABLED == ON
	Timer_InitTask(Timer_LCD_Refresh, 500, &Task_LCD_Refresh);
#endif

	USS_TriggerMeasurement();
}

/* Main task : read USS sensor */
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

#if LCD_ENABLED == ON
		if (average != lcdLastValue)
		{
			sprintf(lcd_buffer, "%d cm", average);
			lcdLastValue = average;
			requestRefresh = TRUE;
		}
#endif

		/* Trigger next acquisition */
		USS_TriggerMeasurement();
	}
}

#if LCD_ENABLED == ON
void Task_LCD_Refresh()
{
	if (requestRefresh == TRUE)
	{
		LCD_ClearDisplay();
		LCD_ReturnHome();
		LCD_PrintString(lcd_buffer);
		requestRefresh = FALSE;
	}
}
#endif

/* Blinks a LED to indicate status */
void Task_Blink()
{
	static PinState pinState = Low;

	pinState = (pinState == Low ? High : Low);
	Port_SetPinState(Pin_LED, pinState);
}

#if BUZZER_ENABLED == ON
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
#endif
