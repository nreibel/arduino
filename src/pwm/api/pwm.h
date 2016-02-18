#ifndef SRC_PWM_API_PWM_H_
#define SRC_PWM_API_PWM_H_

#include "types.h"

void PWM_Init();
void PWM_SetPWM(uint8_t dutyCycle);
void PWM_StopPWM();

#endif /* SRC_PWM_API_PWM_H_ */
