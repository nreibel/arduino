#ifndef __TIMER16_LL_API_H__
#define __TIMER16_LL_API_H__

#include "types.h"

typedef enum {
    TIM16_LL_FALLING_EDGE = 0x0,
    TIM16_LL_RISING_EDGE  = 0x1,
} tim16_edge_t;

typedef enum {
    TIM16_LL_PSCL_NONE = 0x0,
    TIM16_LL_PSCL_1    = 0x1,
    TIM16_LL_PSCL_8    = 0x2,
    TIM16_LL_PSCL_64   = 0x3,
    TIM16_LL_PSCL_256  = 0x4,
    TIM16_LL_PSCL_1024 = 0x5,
} tim16_ll_pscl_t;

typedef union {
    struct {
        uint8_t wgm  : 2;
        uint8_t comc : 2;
        uint8_t comb : 2;
        uint8_t coma : 2;
    } b;
    uint8_t raw;
} tim16_tccra_t;

typedef union {
    struct {
        uint8_t cs   : 3;
        uint8_t wgm  : 2;
        uint8_t      : 1;
        uint8_t ices : 1;
        uint8_t icn  : 1;
    } b;
    uint8_t raw;
} tim16_tccrb_t;

typedef union {
    struct {
        uint8_t      : 5;
        uint8_t focc : 1;
        uint8_t focb : 1;
        uint8_t foca : 1;
    } b;
    uint8_t raw;
} tim16_tccrc_t;

typedef union {
    struct {
        uint8_t tov  : 1;
        uint8_t ocfa : 1;
        uint8_t ocfb : 1;
        uint8_t ocfc : 1;
        uint8_t      : 1;
        uint8_t icf  : 1;
        uint8_t      : 2;
    } b;
    uint8_t raw;
} volatile * tim16_tifr_t;

typedef union {
    struct {
        uint8_t toie  : 1;
        uint8_t ociea : 1;
        uint8_t ocieb : 1;
        uint8_t ociec : 1;
        uint8_t       : 1;
        uint8_t icie  : 1;
        uint8_t       : 2;
    } b;
    uint8_t raw;
} volatile * tim16_timsk_t;

typedef struct {
    tim16_tccra_t tccra;
    tim16_tccrb_t tccrb;
    tim16_tccrc_t tccrc;
    uint8_t __reserved;
    uint16_t tcnt;
    uint16_t icr;
    uint16_t ocra;
    uint16_t ocrb;
    uint16_t ocrc;
} volatile * mem_tim16_t;

#define _TIM1 TYPECAST(0x80, const mem_tim16_t)
#define _TIM3 TYPECAST(0x90, const mem_tim16_t)

#define _TIFR1 TYPECAST(0x036, const tim16_tifr_t)
#define _TIFR3 TYPECAST(0x038, const tim16_tifr_t)

#define _TIMSK1 TYPECAST(0x06F, const tim16_timsk_t)
#define _TIMSK3 TYPECAST(0x071, const tim16_timsk_t)

extern void tim16_ll_icp_cbk(mem_tim16_t timer, tim16_edge_t edge, uint16_t value);
extern void tim16_ll_ovf_cbk(mem_tim16_t timer);

void tim16_ll_enable_interrupts(tim16_timsk_t timsk);
void tim16_ll_reset_value(mem_tim16_t tim);
void tim16_ll_set_prescaler(mem_tim16_t tim, tim16_ll_pscl_t pscl);
void tim16_ll_select_input_capture_edge(mem_tim16_t tim, tim16_edge_t edge);

uint16_t tim16_ll_get_input_capture(mem_tim16_t tim);
uint16_t tim16_ll_get_value(mem_tim16_t tim);

#endif /* __TIMER16_LL_API_H__ */
