#ifndef __ICP_API_H__
#define __ICP_API_H__

#include "types.h"

typedef enum {
    ICP1,
    NUMBER_OF_ICP
} icp_t;

int icp_init(icp_t self);

#endif /* __ICP_API_H__ */
