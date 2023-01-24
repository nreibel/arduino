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

void    eeprom_ll_init                  (eeprom_t self);
bool    eeprom_ll_ready                 (eeprom_t self);
void    eeprom_ll_set_address           (eeprom_t self, uint8_t addr);
void    eeprom_ll_set_data              (eeprom_t self, uint8_t data);
uint8_t eeprom_ll_get_data              (eeprom_t self);
void    eeprom_ll_master_prog_enable    (eeprom_t self);
void    eeprom_ll_read_enable           (eeprom_t self);
void    eeprom_ll_prog_enable           (eeprom_t self);

#endif /* __EEPROM_LL_H__ */
