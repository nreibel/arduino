#ifndef OS_API_OS_H_
#define OS_API_OS_H_

#include "types.h"

#define ARCH_ATTINY84  1
#define ARCH_ATMEGA328 2

#if defined (__AVR_ATtiny84__)
	#define ARCHITECTURE ARCH_ATTINY84
#elif defined (__AVR_ATmega328P__)
	#define ARCHITECTURE ARCH_ATMEGA328
#else
	#error "Architecture is not supported"
#endif

uint32_t Os_GetCurrentTimeMs();
void     Os_Sleep(uint32_t ms);
void     Os_EnableInterrupts();

#endif /* OS_API_OS_H_ */
