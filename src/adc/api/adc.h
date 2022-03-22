#ifndef ADC_API_H__
#define ADC_API_H__

#include "adc_ll.h"

enum {
    ADC_OK,
    ADC_ERROR_INSTANCE,
    ADC_ERROR_NOT_CONFIGURED,
    ADC_ERROR_CHANNEL,
    ADC_ERROR_VREF,
    ADC_ERROR_PRESCALER
};

typedef enum {
    ADC_VREF_AREF,
    ADC_VREF_AVCC,
    ADC_VREF_INTERNAL1V1
} adc_vref_t;

typedef enum {
    ADC_PRESCALER_2,
    ADC_PRESCALER_4,
    ADC_PRESCALER_8,
    ADC_PRESCALER_16,
    ADC_PRESCALER_32,
    ADC_PRESCALER_64,
    ADC_PRESCALER_128
} adc_prescaler_t;

typedef enum {
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7
} adc_channel_t;

typedef struct adc_prv_s {
    adc_t dev;
    bool configured;
} * adc_handle_t;

int adc_init(adc_handle_t self, adc_t dev);
int adc_configure(adc_handle_t self, adc_vref_t vref, adc_prescaler_t pscl);
int adc_read_word(adc_handle_t self, adc_channel_t channel, uint16_t * value);
int adc_read_byte(adc_handle_t self, adc_channel_t channel, uint8_t * value);

#endif /* ADC_API_H__ */
