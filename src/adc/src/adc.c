#include "adc.h"
#include "adc_cfg.h"
#include "types.h"
#include "bits.h"
#include "avr/io.h"

void ADC_Init()
{
    // Poweron ADC
    RESET_BIT(PRR, PRADC);

    // Select Vref
    ADMUX = MASK(ADC_VREF_SELECT, 0x3) << 6;

    // Enable ADC, set prescaler
    ADCSRA = BIT(ADEN) | MASK(ADC_PRESCALER_SELECT, 0x7);
}

uint16_t ADC_Read(uint8_t channel)
{
    SET_BITS(ADMUX, channel, 0x0F);
    SET_BIT(ADCSRA, ADSC);

    // Wait for acquisition
    while(IS_SET_BIT(ADCSRA, ADSC));

    return ADC;
}
