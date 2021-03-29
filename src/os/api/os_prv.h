#ifndef __OS_PRV_H__
#define __OS_PRV_H__

#include "types.h"

void os_init();
void os_sleep();
void os_interrupts_enable();
void os_interrupts_disable();
void os_timer_callback();

#endif /* __OS_PRV_H__ */
