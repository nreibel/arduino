#ifndef SRC_PWM_API_PWM_H_
#define SRC_PWM_API_PWM_H_

#include "types.h"

typedef enum {
    PWM_5,
    PWM_6,
} PWM;

void PWM_Init   (PWM pin, bool inverted);
void PWM_StopPWM(PWM pin);
void PWM_SetPWM (PWM pin, uint8_t dutyCycle);

#endif /* SRC_PWM_API_PWM_H_ */
