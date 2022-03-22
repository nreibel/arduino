#ifndef __EEPROM_API_H__
#define __EEPROM_API_H__

#include "types.h"

enum {
    EEPROM_OK,
    EEPROM_ERROR
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

int eeprom_init();
int eeprom_write(uint16_t addr, void* data, unsigned int length);
int eeprom_read(uint16_t addr, void* data, unsigned int length);
int eeprom_memset(uint16_t addr, uint8_t val, unsigned int length);

#endif /* __EEPROM_API_H__ */
