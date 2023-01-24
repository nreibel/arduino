#ifndef __EEPROM_API_H__
#define __EEPROM_API_H__

#include "types.h"
#include "eeprom_ll.h"

enum {
    EEPROM_OK
};

int eeprom_init         (eeprom_t self);
int eeprom_write_byte   (eeprom_t self, uint16_t addr, uint8_t byte);
int eeprom_write        (eeprom_t self, uint16_t addr, const void * data, unsigned int length);
int eeprom_read_byte    (eeprom_t self, uint16_t addr, uint8_t * data);
int eeprom_read         (eeprom_t self, uint16_t addr, void * data, unsigned int length);
int eeprom_memset       (eeprom_t self, uint16_t addr, uint8_t val, unsigned int length);
int eeprom_erase        (eeprom_t self, uint16_t addr, unsigned int length);

#endif /* __EEPROM_API_H__ */
