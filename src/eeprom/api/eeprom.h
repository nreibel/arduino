#ifndef __EEPROM_API_H__
#define __EEPROM_API_H__

#include "types.h"

int eeprom_init();
int eeprom_write(uint16_t addr, void* data, unsigned int length);
int eeprom_read(uint16_t addr, void* data, unsigned int length);
int eeprom_memset(uint16_t addr, uint8_t val, unsigned int length);

#endif /* __EEPROM_API_H__ */
