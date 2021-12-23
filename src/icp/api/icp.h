#ifndef __ICP_API_H__
#define __ICP_API_H__

/*
 * Make use of Timer 1 Input Capture Mode to measure frequency and duty cycle on pin ICP1
 *
 * Readings with interrupts can be accurate up to ~2kHz, depending on CPU load
 * Readings in blocking mode can be accurate (+/-5%) up to 50kHz BUT will hang if there are no events on ICP1
 *
 * TODO : implement timeout on blocking calls
 */

#include "types.h"

typedef enum {
    ICP_OK,
    ICP_ERROR,
    ICP_ERROR_INSTANCE,
    ICP_ERROR_PARAM,
    ICP_ERROR_NO_DATA,
    ICP_ERROR_OVERFLOW
} icp_error_t;

typedef enum {
    ICP1, // PB0
    NUMBER_OF_ICP
} icp_t;

typedef enum {
    ICP_PRESCALER_1,
    ICP_PRESCALER_8,
    ICP_PRESCALER_64,
    ICP_PRESCALER_256,
    ICP_PRESCALER_1024
} icp_prescaler_t;

int icp_init(icp_t self, icp_prescaler_t prescaler, bool useInterrupts);
int icp_get_duty_cycle(icp_t self, uint8_t * duty_cycle);
int icp_get_frequency(icp_t self, uint16_t * frequency);

#endif /* __ICP_API_H__ */
