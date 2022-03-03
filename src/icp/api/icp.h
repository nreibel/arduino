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

enum {
    ICP_OK,
    ICP_ERROR,
    ICP_ERROR_INSTANCE,
    ICP_ERROR_OVERFLOW,
    ICP_ERROR_PRESCALER,
};

typedef enum {
    ICP_PRESCALER_NONE = 0x0,
    ICP_PRESCALER_1    = 0x1,
    ICP_PRESCALER_8    = 0x2,
    ICP_PRESCALER_64   = 0x3,
    ICP_PRESCALER_256  = 0x4,
    ICP_PRESCALER_1024 = 0x5
} icp_prescaler_t;

typedef union {
    struct {
        uint8_t wgm        : 2;
        uint8_t __reserved : 2;
        uint8_t comb       : 2;
        uint8_t coma       : 2;
    } bits;
    uint8_t reg;
} tccr1a_t;

typedef union {
    struct {
        uint8_t cs         : 3;
        uint8_t wgm        : 2;
        uint8_t __reserved : 1;
        uint8_t ices       : 1;
        uint8_t icnc       : 1;
    } bits;
    uint8_t reg;
} tccr1b_t;

typedef union {
    struct {
        uint8_t __reserved : 6;
        uint8_t focb       : 1;
        uint8_t foca       : 1;
    } bits;
    uint8_t reg;
} tccr1c_t;

typedef union {
    struct {
        uint8_t toie        : 1;
        uint8_t ociea       : 1;
        uint8_t ocieb       : 1;
        uint8_t __reserved1 : 2;
        uint8_t icie        : 1;
        uint8_t __reserved2 : 2;
    } bits;
    uint8_t reg;
} timsk1_t;

typedef union {
    struct {
        uint8_t tov         : 1;
        uint8_t ocfa        : 1;
        uint8_t ocfb        : 1;
        uint8_t __reserved1 : 2;
        uint8_t icf         : 1;
        uint8_t __reserved2 : 2;
    } bits;
    uint8_t reg;
} tifr1_t;

typedef struct {
    tccr1a_t tccra;
    tccr1b_t tccrb;
    tccr1c_t tccrc;
    uint8_t  __reserved;
    uint16_t tcnt;
    uint16_t icr;
    uint16_t ocra;
    uint16_t ocrb;
} volatile * icp_t;

typedef struct icp_handle_prv_s {
    icp_t instance;
    tifr1_t tifr;
    uint8_t factor;
    uint8_t prescaler;
} * icp_handle_t;

#define ICP1 TYPECAST(0x80, const icp_t)

int icp_init(icp_handle_t self, icp_t icp, icp_prescaler_t prescaler);
int icp_get_duty_cycle(icp_handle_t self, uint8_t * duty_cycle);
int icp_get_frequency(icp_handle_t self, uint16_t * frequency);

void icp_ll_init(icp_t icp);
void icp_ll_reset(icp_t icp);
uint16_t icp_ll_capture_edge(icp_t icp, bool rising);
bool icp_ll_is_ovf(icp_t icp);
void icp_ll_clear_ovf(icp_t icp);
void icp_ll_set_interrupt_enabled(icp_t self, bool enabled);
void icp_ll_set_prescaler(icp_t icp, uint8_t prescaler);

#endif /* __ICP_API_H__ */
