#ifndef TIMER_API_H__
#define TIMER_API_H__

#include "types.h"
#include "gpio.h"
#include "timer_ll.h"

typedef enum {
    TIMER_OK,
    TIMER_ERROR,
    TIMER_ERROR_INSTANCE,
    TIMER_ERROR_INIT,
    TIMER_ERROR_MODE,
    TIMER_ERROR_OCA_MODE,
    TIMER_ERROR_OUTPUT_MODE,
    TIMER_ERROR_PRESCALER,
} timer_error_t;

typedef enum {
    TIMER_MODE_NORMAL,
    TIMER_MODE_PHASE_CORRECT_PWM,
    TIMER_MODE_CTC,
    TIMER_MODE_FAST_PWM
} timer_mode_t;

typedef enum {
    TIMER_PRESCALER_STOPPED,
    TIMER_PRESCALER_1,
    TIMER_PRESCALER_8,
    TIMER_PRESCALER_32,
    TIMER_PRESCALER_64,
    TIMER_PRESCALER_128,
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
    const mem_timer_t instance;
    timer_prescaler_t prescaler;
    struct gpio_prv_s oca;
    struct gpio_prv_s ocb;
    bool init;
} timer_handle_t;

typedef struct {
    timer_mode_t mode;
    timer_output_mode_t output_mode_a;
    timer_output_mode_t output_mode_b;
    timer_prescaler_t prescaler;
    timer_oca_mode_t oca_mode;
    timer_interrupt_t imask;
    uint8_t ocra;
    uint8_t ocrb;
} timer_config_t;

int timer_init(timer_t handle, timer_config_t * config);
int timer_start(timer_t handle);
int timer_stop(timer_t handle);
int timer_set_ocra(timer_t handle, uint8_t val);
int timer_set_ocrb(timer_t handle, uint8_t val);

#endif /* TIMER_API_H__ */
