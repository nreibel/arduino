#ifndef SRC_PWM_API_PWM_H_
#define SRC_PWM_API_PWM_H_

#include "types.h"

typedef enum {
    PWM_5,
    PWM_6,
    NUMBER_OF_PWM_CHANNELS
} pwm_t;

typedef enum {
    PWM_PRESCALER_0,
    PWM_PRESCALER_8,
    PWM_PRESCALER_64,
    PWM_PRESCALER_256,
    PWM_PRESCALER_1024
} pwm_prescaler_t;

typedef enum {
    PWM_MODE_FAST_PWM,
    PWM_MODE_PHASE_CORRECT,
} pwm_mode_t;

void pwm_init(pwm_mode_t mode, pwm_prescaler_t cs);
bool pwm_start(pwm_t self, uint8_t duty_cycle, bool inverted);
bool pwm_set_duty_cycle(pwm_t self, uint8_t duty_cycle);
bool pwm_stop(pwm_t self);

#endif /* SRC_PWM_API_PWM_H_ */
