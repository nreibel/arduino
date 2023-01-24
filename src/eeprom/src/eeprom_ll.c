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

void eeprom_ll_set_address(eeprom_t self, uint8_t addr)
{
    self->eear = addr;
}

void eeprom_ll_set_data(eeprom_t self, uint8_t data)
{
    self->eedr = data;
}

uint8_t eeprom_ll_get_data(eeprom_t self)
{
    return self->eedr;
}

void eeprom_ll_master_prog_enable(eeprom_t self)
{
    self->eecr.bits.eempe = 1;
}

void eeprom_ll_prog_enable(eeprom_t self)
{
    self->eecr.bits.eepe = 1;
}

void eeprom_ll_read_enable(eeprom_t self)
{
    self->eecr.bits.eere = 1;
}
