#ifndef TIMER_LL_API_H__
#define TIMER_LL_API_H__

#include "types.h"
#include "timer.h"

enum {
    TIMER_LL_OK,
    TIMER_LL_ERROR_INSTANCE,
    TIMER_LL_ERROR_PRESCALER
};

enum {
    TIMER_LL_WGM_NORMAL            = 0x00,
    TIMER_LL_WGM_PHASE_CORRECT_PWM = 0x01,
    TIMER_LL_WGM_CTC               = 0x02,
    TIMER_LL_WGM_FAST_PWM          = 0x03
};

enum {
    TIMER_LL_COM_DISABLED = 0x0, // OC pin disconnected
    TIMER_LL_COM_TOGGLE   = 0x1, // Toggle OC pin on Compare Match
    TIMER_LL_COM_DEFAULT  = 0x2, // Clear OC pin on Compare Match
    TIMER_LL_COM_INVERTED = 0x3, // Set OC pin on Compare Match
};

enum{
    TIMER0_LL_CS_STOPPED = 0x0,
    TIMER0_LL_CS_CLKIO   = 0x1,
    TIMER0_LL_CS_DIV8    = 0x2,
    TIMER0_LL_CS_DIV64   = 0x3,
    TIMER0_LL_CS_DIV256  = 0x4,
    TIMER0_LL_CS_DIV1024 = 0x5
};

enum{
    TIMER2_LL_CS_STOPPED = 0x0,
    TIMER2_LL_CS_CLKIO   = 0x1,
    TIMER2_LL_CS_DIV8    = 0x2,
    TIMER2_LL_CS_DIV32   = 0x3,
    TIMER2_LL_CS_DIV64   = 0x4,
    TIMER2_LL_CS_DIV128  = 0x5,
    TIMER2_LL_CS_DIV256  = 0x6,
    TIMER2_LL_CS_DIV1024 = 0x7
};

enum {
    TIMER_LL_INT_OVF   = 0x1,
    TIMER_LL_INT_OCM_A = 0x2,
    TIMER_LL_INT_OCM_B = 0x4
};

typedef union {
    struct {
        uint8_t wgm        : 2;
        uint8_t __reserved : 2;
        uint8_t comb       : 2;
        uint8_t coma       : 2;
    } bits;
    uint8_t reg;
} tccra_t;

typedef union {
    struct {
        uint8_t cs         : 3;
        uint8_t wgm        : 1;
        uint8_t __reserved : 2;
        uint8_t focb       : 1;
        uint8_t foca       : 1;
    } bits;
    uint8_t reg;
} tccrb_t;

typedef union {
    struct {
        uint8_t toie       : 1;
        uint8_t ociea      : 1;
        uint8_t ocieb      : 1;
        uint8_t __reserved : 5;
    } bits;
    uint8_t reg;
} timsk_t;

typedef struct {
    volatile tccra_t tccra;
    volatile tccrb_t tccrb;
    volatile uint8_t tcnt;
    volatile uint8_t ocra;
    volatile uint8_t ocrb;
} * mem_timer_t;

// Interrupt handler
extern void timer_interrupt(timer_t timer);

int timer_ll_reset(timer_t timer);
int timer_ll_power_enable(timer_t timer);
int timer_ll_set_imask(timer_t timer, uint8_t imask);
int timer_ll_set_prescaler(timer_t timer, uint8_t prescaler);
int timer_ll_set_wgm(timer_t timer, uint8_t wgm);
int timer_ll_set_oca_mode(timer_t timer, bool top);
int timer_ll_set_ocra(timer_t timer, uint8_t val);
int timer_ll_set_ocrb(timer_t timer, uint8_t val);
int timer_ll_set_coma(timer_t timer, uint8_t val);
int timer_ll_set_comb(timer_t timer, uint8_t val);

int timer_ll_enable_oca(timer_t timer);
int timer_ll_enable_ocb(timer_t timer);

#endif /* TIMER_LL_API_H__ */
