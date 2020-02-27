#ifndef SRC_ADC_API_ADC_H_
#define SRC_ADC_API_ADC_H_

#include "types.h"

typedef enum {
    ADC_VREF_AREF = 0,
    ADC_VREF_AVCC,
    ADC_VREF_RESERVED,
    ADC_VREF_INTERNAL1V1
} ADC_Vref;

typedef enum {
    ADC_PRESCALER_2 = 1,
    ADC_PRESCALER_4,
    ADC_PRESCALER_8,
    ADC_PRESCALER_16,
    ADC_PRESCALER_32,
    ADC_PRESCALER_64,
    ADC_PRESCALER_128
} ADC_Prescaler;

void ADC_Init();
uint16_t ADC_Read(uint8_t channel);

#endif /* SRC_ADC_API_ADC_H_ */
