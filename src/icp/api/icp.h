#ifndef __ICP_API_H__
#define __ICP_API_H__

#include "types.h"

typedef enum {
    ICP_OK,
    ICP_ERROR,
    ICP_ERROR_INSTANCE,
    ICP_ERROR_OVERFLOW
} icp_error_t;

typedef enum {
    ICP1,
    NUMBER_OF_ICP
} icp_t;

typedef enum {
    ICP_PRESCALER_1,
    ICP_PRESCALER_8,
    ICP_PRESCALER_64,
    ICP_PRESCALER_256,
    ICP_PRESCALER_1024
} icp_prescaler_t;

int icp_init(icp_t self, icp_prescaler_t prescaler);
int icp_get_duty_cycle(icp_t self, float * duty_cycle);

#endif /* __ICP_API_H__ */
