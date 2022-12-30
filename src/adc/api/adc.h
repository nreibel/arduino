#ifndef ADC_API_H__
#define ADC_API_H__

#include "adc_ll.h"
#include "os_cfg.h"

enum {
    ADC_OK,
    ADC_ERR_PARAM,
    ADC_ERR_INIT,
    ADC_ERR_BUSY,
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
    ADC_CHANNEL_7,
    NUMBER_OF_ADC_CHANNELS
} adc_channel_t;

typedef enum {
    ADC_TRIGGER_DISABLED,
    ADC_TRIGGER_FREE_RUNNING,
    ADC_TRIGGER_ANALOG_COMPARATOR,
    ADC_TRIGGER_EXTERNAL_IRQ_0,
    ADC_TRIGGER_TIMER0_COMPARE_MATCH_A,
    ADC_TRIGGER_TIMER0_OVERFLOW,
    ADC_TRIGGER_TIMER1_COMPARE_MATCH_B,
    ADC_TRIGGER_TIMER1_OVERFLOW,
    ADC_TRIGGER_TIMER1_CAPTURE_EVENT
} adc_trigger_t;

typedef struct adc_prv_s {
    adc_t dev;
    bool configured;
    bool started;
} * adc_handle_t;

#if OS_MALLOC
adc_handle_t adc_create(void);
void adc_destroy(adc_handle_t dev);
#endif // OS_MALLOC

int adc_init(adc_handle_t self, adc_t dev);
int adc_configure(adc_handle_t self, adc_vref_t vref, adc_prescaler_t pscl);
int adc_setup_trigger(adc_handle_t self, adc_trigger_t trigger);
int adc_read_word(adc_handle_t self, adc_channel_t channel, uint16_t * value);
int adc_read_byte(adc_handle_t self, adc_channel_t channel, uint8_t * value);

#endif /* ADC_API_H__ */
