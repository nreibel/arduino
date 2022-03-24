#include "eeprom.h"

// TODO : use programming modes to preserve EEPROM and minimize write times

int eeprom_init(eeprom_t self)
{
    if (self == NULL_PTR)
        return -EEPROM_ERROR_INSTANCE;

    eeprom_ll_init(self);

    return EEPROM_OK;
}

int eeprom_memset(eeprom_t self, uint16_t addr, uint8_t val, unsigned int length)
{
    if (self == NULL_PTR)
        return -EEPROM_ERROR_INSTANCE;

    for (unsigned int i = 0 ; i < length ; i++)
    {
        eeprom_ll_wait_ready(self);
        eeprom_ll_write_byte(self, addr++, val);
    }

    return length;
}

int eeprom_write(eeprom_t self, uint16_t addr, const void * data, unsigned int length)
{
    if (self == NULL_PTR)
        return -EEPROM_ERROR_INSTANCE;

    const uint8_t * bytes = data;

    for (unsigned int i = 0 ; i < length ; i++)
    {
        eeprom_ll_wait_ready(self);
        eeprom_ll_write_byte(self, addr++, bytes[i]);
    }

    return length;
}

int eeprom_read(eeprom_t self, uint16_t addr, void* data, unsigned int length)
{
    if (self == NULL_PTR)
        return -EEPROM_ERROR_INSTANCE;

    uint8_t * bytes = data;

    eeprom_ll_wait_ready(self);

    for (unsigned int i = 0 ; i < length ; i++)
    {
        bytes[i] = eeprom_ll_read_byte(self, addr++);
    }

    return length;
}

int eeprom_erase(eeprom_t self, uint16_t addr, unsigned int length)
{
    if (self == NULL_PTR)
        return -EEPROM_ERROR_INSTANCE;

    return eeprom_memset(self, addr, EEPROM_ERASE_VALUE, length);
}
