#include "eeprom.h"
#include "bits.h"
#include "types.h"
#include <avr/io.h>

int eeprom_init()
{
    /* Erase and write in atomic operation */
    EECR = 0;

    return 0;
}

int eeprom_memset(uint16_t addr, uint8_t val, unsigned int length)
{
    int count = 0;

    while (length-- > 0)
    {
        EEAR = addr++;
        EEDR = val;

        /* Set Master Program Enable first, then Program Enable */
        SET_BIT(EECR, EEMPE);
        SET_BIT(EECR, EEPE);

        while (IS_SET_BIT(EECR, EEPE));

        count++;
    }

    return count;
}

int eeprom_write(uint16_t addr, void* data, unsigned int length)
{
    int count = 0;

    while (length-- > 0)
    {
        EEAR = addr++;
        EEDR = READ_PU8(data++);

        /* Set Master Program Enable first, then Program Enable */
        SET_BIT(EECR, EEMPE);
        SET_BIT(EECR, EEPE);

        while (IS_SET_BIT(EECR, EEPE));

        count++;
    }

    return count;
}

int eeprom_read(uint16_t addr, void* data, unsigned int length)
{
    int count = 0;

    while(length-- > 0)
    {
        EEAR = addr++;

        /* Read Enable */
        SET_BIT(EECR, EERE);

        WRITE_PU8(data++, EEDR);

        count++;
    }

    return count;
}
