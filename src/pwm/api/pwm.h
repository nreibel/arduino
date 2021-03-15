#ifndef SRC_PWM_API_PWM_H_
#define SRC_PWM_API_PWM_H_

#include "types.h"

typedef enum {
    PWM_5,
    PWM_6,
    NUMBER_OF_PWM_CHANNELS
} pwm_t;

void pwm_init();
bool pwm_start(pwm_t self, uint8_t duty_cycle, bool inverted);
bool pwm_set_duty_cycle(pwm_t self, uint8_t duty_cycle);
bool pwm_stop(pwm_t self);

#endif /* SRC_PWM_API_PWM_H_ */
