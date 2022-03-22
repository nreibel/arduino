#include "eeprom.h"
#include "bits.h"
#include "types.h"

int eeprom_init()
{
    /* Erase and write in atomic operation */
    EEPROM0->eecr.reg = 0;
    return EEPROM_OK;
}

int eeprom_memset(uint16_t addr, uint8_t val, unsigned int length)
{
    for (unsigned int i = 0 ; i < length ; i++)
    {
        // Wait for completion of previous write
        while(EEPROM0->eecr.bits.eepe);

        // Setup address and data
        EEPROM0->eear = addr++;
        EEPROM0->eedr = val;

        // Set Master Program Enable first, then Program Enable
        EEPROM0->eecr.bits.eempe = 1;
        EEPROM0->eecr.bits.eepe = 1;
    }

    return length;
}

int eeprom_write(uint16_t addr, void* data, unsigned int length)
{
    uint8_t * bytes = data;

    for (unsigned int i = 0 ; i < length ; i++)
    {
        // Wait for completion of previous write
        while(EEPROM0->eecr.bits.eepe);

        // Setup address and data
        EEPROM0->eear = addr++;
        EEPROM0->eedr = bytes[i];

        // Set Master Program Enable first, then Program Enable
        EEPROM0->eecr.bits.eempe = 1;
        EEPROM0->eecr.bits.eepe = 1;
    }

    return length;
}

int eeprom_read(uint16_t addr, void* data, unsigned int length)
{
    uint8_t * bytes = data;

    // Wait for completion of previous write
    while(EEPROM0->eecr.bits.eepe);

    for (unsigned int i = 0 ; i < length ; i++)
    {
        EEPROM0->eear = addr++;
        EEPROM0->eecr.bits.eere = 1;
        bytes[i] = EEPROM0->eedr;
    }

    return length;
}
