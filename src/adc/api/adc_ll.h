#ifndef ADC_LL_H__
#define ADC_LL_H__

#include "types.h"

#define ADC_RESOLUTION 10U
#define ADC_MAX_VALUE  ((1U << (ADC_RESOLUTION)) - 1)

enum {
    ADC_LL_OK,
    ADC_LL_ERROR
};

enum {
    ADC_LL_VREF_AREF        = 0x0,
    ADC_LL_VREF_AVCC        = 0x1,
    ADC_LL_VREF_RESERVED    = 0x2,
    ADC_LL_VREF_INTERNAL1V1 = 0x3
};

enum {
    ADC_LL_PSCL_2   = 0x1,
    ADC_LL_PSCL_4   = 0x2,
    ADC_LL_PSCL_8   = 0x3,
    ADC_LL_PSCL_16  = 0x4,
    ADC_LL_PSCL_32  = 0x5,
    ADC_LL_PSCL_64  = 0x6,
    ADC_LL_PSCL_128 = 0x7
};

typedef union {
    struct {
        uint8_t adps  : 3;
        uint8_t adie  : 1;
        uint8_t adif  : 1;
        uint8_t adate : 1;
        uint8_t adsc  : 1;
        uint8_t aden  : 1;
    } bits;
    uint8_t reg;
} adcsra_t;

typedef union {
    struct {
        uint8_t mux        : 4;
        uint8_t __reserved : 1;
        uint8_t adlar      : 1;
        uint8_t refs       : 2;
    } bits;
    uint8_t reg;
} admux_t;

typedef struct {
    volatile union {
        uint8_t bytes[2];
        uint16_t u16;
    } adc;
    volatile adcsra_t  adcsra;
    volatile uint8_t   adcsrb;
    volatile admux_t   admux;
} * adc_t;

#define ADC0 TYPECAST(0x78, const adc_t)

extern void adc_interrupt(adc_t adc, uint16_t value);

int adc_ll_init(adc_t adc);
int adc_ll_set_enabled(adc_t adc, bool enabled);
int adc_ll_set_interrupts_enabled(adc_t adc, bool enabled);
int adc_ll_set_prescaler(adc_t adc, uint8_t pscl);
int adc_ll_set_vref(adc_t adc, uint8_t vref);
int adc_ll_set_left_aligned(adc_t adc, bool left_aligned);
int adc_ll_select_channel(adc_t adc, uint8_t channel);
int adc_ll_start_conversion(adc_t adc);
int adc_ll_wait_conversion(adc_t adc);
int adc_ll_read_word(adc_t adc, uint16_t * data);
int adc_ll_read_byte(adc_t adc, uint8_t * data);

#endif /* ADC_LL_H__ */