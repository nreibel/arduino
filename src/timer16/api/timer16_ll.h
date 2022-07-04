#ifndef __TIMER16_LL_API_H__
#define __TIMER16_LL_API_H__

#include "types.h"

enum {
    TIMER16_LL_OK,
    TIMER16_LL_ERROR_EDGE
};

typedef enum {
    TIM16_LL_RISING_EDGE,
    TIM16_LL_FALLING_EDGE
} tim16_edge_t;

typedef enum {
    TIM16_LL_PRESCALER_NONE,
    TIM16_LL_PRESCALER_1,
    TIM16_LL_PRESCALER_8,
    TIM16_LL_PRESCALER_64,
    TIM16_LL_PRESCALER_256,
    TIM16_LL_PRESCALER_1024,
} tim16_prescaler_t;

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

#define TIM1 TYPECAST(0x80, const mem_tim16_t)
#define TIM3 TYPECAST(0x90, const mem_tim16_t)

#define _TIFR1 TYPECAST(0x036, const tim16_tifr_t)
#define _TIFR3 TYPECAST(0x038, const tim16_tifr_t)

void tim16_ll_reset_value(mem_tim16_t tim);
int tim16_ll_set_prescaler(mem_tim16_t tim, tim16_prescaler_t pscl);
int tim16_ll_select_input_capture_edge(mem_tim16_t tim, tim16_edge_t edge);

uint16_t tim16_ll_get_input_capture(mem_tim16_t tim);
uint16_t tim16_ll_get_value(mem_tim16_t tim);

bool tim16_ll_check_ovf(tim16_tifr_t tifr);
bool tim16_ll_check_icf(tim16_tifr_t tifr);

void tim16_ll_reset_ovf(tim16_tifr_t tifr);
void tim16_ll_reset_icf(tim16_tifr_t tifr);

#endif /* __TIMER16_LL_API_H__ */
