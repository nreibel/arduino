#include "eeprom_ll.h"

// TODO : use programming modes to preserve EEPROM and minimize write times

void eeprom_ll_init(eeprom_t self)
{
    self->eecr.reg = 0;
}

bool eeprom_ll_ready(eeprom_t self)
{
    return self->eecr.bits.eepe == 0;
}

void eeprom_ll_wait_ready(eeprom_t self)
{
    while(!eeprom_ll_ready(self)){}
}

void eeprom_ll_write_byte(eeprom_t self, uint16_t addr, uint8_t byte)
{
    self->eear = addr;
    self->eedr = byte;

    self->eecr.bits.eempe = 1;
    self->eecr.bits.eepe = 1;
}

uint8_t eeprom_ll_read_byte(eeprom_t self, uint16_t addr)
{
    self->eear = addr;
    self->eecr.bits.eere = 1;
    return self->eedr;
}
