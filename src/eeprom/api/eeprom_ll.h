#ifndef __EEPROM_LL_H__
#define __EEPROM_LL_H__

#include "types.h"

#define EEPROM_ERASE_VALUE 0xFF

enum {
    EEPROM_LL_PROG_MODE_ATOMIC     = 0x00,
    EEPROM_LL_PROG_MODE_ERASE_ONLY = 0x01,
    EEPROM_LL_PROG_MODE_WRITE_ONLY = 0x02
};

typedef union {
    struct {
        uint8_t eere       : 1;
        uint8_t eepe       : 1;
        uint8_t eempe      : 1;
        uint8_t eerie      : 1;
        uint8_t eepm       : 2;
        uint8_t __reserved : 2;
    } bits;
    uint8_t reg;
} eecr_t;

typedef struct {
    eecr_t   eecr;
    uint8_t  eedr;
    uint16_t eear;
} volatile * eeprom_t;

#define EEPROM0 TYPECAST(0x3F, eeprom_t)

void eeprom_ll_init(eeprom_t self);
bool eeprom_ll_ready(eeprom_t self);
void eeprom_ll_wait_ready(eeprom_t self);
void eeprom_ll_erase_byte(eeprom_t self, uint16_t addr);
void eeprom_ll_write_byte(eeprom_t self, uint16_t addr, uint8_t byte);
uint8_t eeprom_ll_read_byte(eeprom_t self, uint16_t addr);

#endif /* __EEPROM_LL_H__ */
