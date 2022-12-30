#ifndef __OS_MACROS_API_H__
#define __OS_MACROS_API_H__

#include "os_cfg.h"

extern void error_trap(void);

#define CHECK_NULL(_obj) if (OS_CHECK_NULL && !_obj) { error_trap(); }

#endif /* __OS_MACROS_API_H__ */
