#ifndef __TIMER_API_H__
#define __TIMER_API_H__

#include "types.h"

typedef enum {
    TIMER_OK,
    TIMER_ERROR,
    TIMER_ERROR_INSTANCE,
    TIMER_ERROR_NOT_INIT,
    TIMER_ERROR_MODE,
    TIMER_ERROR_OCA_MODE,
    TIMER_ERROR_OUTPUT_MODE,
    TIMER_ERROR_PRESCALER,
} timer_error_t;

typedef enum {
    TIMER_0,
    // TIMER_1, // TODO
    TIMER_2,
    NUMBER_OF_TIMERS
} timer_t;

typedef enum {
    TIMER_MODE_NORMAL,
    TIMER_MODE_PHASE_CORRECT_PWM,
    TIMER_MODE_CTC,
    TIMER_MODE_FAST_PWM
} timer_mode_t;

typedef enum {
    TIMER_PRESCALER_STOPPED,
    TIMER_PRESCALER_0,
    TIMER_PRESCALER_8,
    TIMER_PRESCALER_32, // Timer 2 only
    TIMER_PRESCALER_64,
    TIMER_PRESCALER_128, // Timer 2 only
    TIMER_PRESCALER_256,
    TIMER_PRESCALER_1024
} timer_prescaler_t;

typedef enum {
    TIMER_OUTPUT_MODE_DISABLED, // OC pin disconnected
    TIMER_OUTPUT_MODE_TOGGLE,   // Toggle OC pin on Compare Match
    TIMER_OUTPUT_MODE_DEFAULT,  // Clear OC pin on Compare Match
    TIMER_OUTPUT_MODE_INVERTED, // Set OC pin on Compare Match
} timer_output_mode_t;

typedef enum {
    TIMER_OCA_MODE_PWM, // OCA and OCB are independent
    TIMER_OCA_MODE_TOP, // OCRA is TOP value for OCB, OCA duty cycle fixed at 50%
} timer_oca_mode_t;

typedef enum {
    TIMER_INTERRUPT_OVERFLOW,
    TIMER_INTERRUPT_OCM_A,
    TIMER_INTERRUPT_OCM_B,
} timer_interrupt_t;

typedef struct {
    timer_mode_t mode;
    timer_output_mode_t output_mode;
    timer_prescaler_t prescaler;
    timer_oca_mode_t oca_mode;
    timer_interrupt_t imask;
    uint8_t ocra;
    uint8_t ocrb;
} timer_config_t;

int timer_init(timer_t self, timer_config_t * config);
int timer_start(timer_t self);
int timer_stop(timer_t self);

#endif /* __TIMER_API_H__ */
