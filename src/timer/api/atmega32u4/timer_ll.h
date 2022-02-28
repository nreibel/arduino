#ifndef TIMER_LL_API_H__
#define TIMER_LL_API_H__

#include "types.h"

typedef enum {
    TIMER_0,
    NUMBER_OF_TIMERS
} timer_t;

typedef enum {
    TIMER_LL_OK,
    TIMER_LL_ERROR_INSTANCE,
    TIMER_LL_ERROR_PRESCALER
} timer_ll_error_t;

typedef union {
    struct {
        uint8_t WGM        : 2;
        uint8_t __reserved : 2;
        uint8_t COMB       : 2;
        uint8_t COMA       : 2;
    } bits;
    uint8_t reg;
} tccra_t;

typedef union {
    struct {
        uint8_t CS         : 3;
        uint8_t WGM        : 1;
        uint8_t __reserved : 2;
        uint8_t FOCB       : 1;
        uint8_t FOCA       : 1;
    } bits;
    uint8_t reg;
} tccrb_t;

typedef union {
    struct {
        uint8_t TOIE       : 1;
        uint8_t OCIEA      : 1;
        uint8_t OCIEB      : 1;
        uint8_t __reserved : 5;
    } bits;
    uint8_t reg;
} timsk_t;

typedef struct {
    volatile tccra_t TCCRA;
    volatile tccrb_t TCCRB;
    volatile uint8_t TCNT;
    volatile uint8_t OCRA;
    volatile uint8_t OCRB;
} * mem_timer_t;

#define TIM0 TYPECAST(0x44, const mem_timer_t)

#endif /* TIMER_LL_API_H__ */