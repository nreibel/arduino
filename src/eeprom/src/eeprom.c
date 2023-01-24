#include "eeprom.h"

// TODO : use programming modes to preserve EEPROM and minimize write times

int eeprom_init(eeprom_t self)
{
    eeprom_ll_init(self);
    return EEPROM_OK;
}

int eeprom_memset(eeprom_t self, uint16_t addr, uint8_t val, unsigned int length)
{
    for (unsigned int i = 0 ; i < length ; i++)
    {
        eeprom_write_byte(self, addr+i, val);
        while(!eeprom_ll_ready(self));
    }

    return length;
}

int eeprom_write_byte(eeprom_t self, uint16_t addr, uint8_t byte)
{
    eeprom_ll_set_address(self, addr);
    eeprom_ll_set_data(self, byte);
    eeprom_ll_master_prog_enable(self);
    eeprom_ll_prog_enable(self);
    return 1;
}

int eeprom_read_byte(eeprom_t self, uint16_t addr, uint8_t * data)
{
    eeprom_ll_set_address(self, addr);
    eeprom_ll_read_enable(self);
    *data = eeprom_ll_get_data(self);
    return 1;
}

int eeprom_write(eeprom_t self, uint16_t addr, const void * data, unsigned int length)
{
    int written = 0;

    for (unsigned int i = 0 ; i < length ; i++)
    {
        written += eeprom_write_byte(self, addr+i, PU8(data)[i]);
        while(!eeprom_ll_ready(self));
    }

    return written;
}

int eeprom_read(eeprom_t self, uint16_t addr, void * data, unsigned int length)
{
    int read = 0;

    for (unsigned int i = 0 ; i < length ; i++)
    {
        read += eeprom_read_byte(self, addr+i, &PU8(data)[i]);
    }

    return read;
}

int eeprom_erase(eeprom_t self, uint16_t addr, unsigned int length)
{
    return eeprom_memset(self, addr, EEPROM_ERASE_VALUE, length);
}
