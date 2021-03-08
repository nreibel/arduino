#include "os.h"
#include "i2c_slave.h"

// read-only I2C registers at separate hardcoded offsets
#define REGISTER_VERSION_MAJOR 0xFD
#define REGISTER_VERSION_MINOR 0xFE
#define REGISTER_VERSION_PATCH 0xFF

// Read-only, simulate some PMBUS data
#define REGISTER_PIN       0x97
#define REGISTER_MFG_MODEL 0x9A
#define MFG_MODEL_LEN      16
const uint16_t pin_linear11 = 0xE920; // 36W in LINEAR11 format
const uint8_t mfg_model[MFG_MODEL_LEN] = "HELLO DUMMY";

// Read/write I2C registers in a continuous address range 0x00-0x0F
uint8_t registers[16] = {0};

/*
What I2C dump should look like :

     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f    0123456789abcdef
00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
10: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
20: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
30: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
40: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
50: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
60: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
70: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
80: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
90: ff ff ff ff ff ff ff 20 e9 ff 48 45 4c 4c 4f 20    ....... ?.HELLO
a0: 44 55 4d 4d 59 00 00 00 00 00 ff ff ff ff ff ff    DUMMY...........
b0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
c0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
d0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
e0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
f0: ff ff ff ff ff ff ff ff ff ff ff ff ff 01 00 00    .............?..
*/

uint8_t I2C_Slave_TransmitCallback(unsigned int offset)
{
    if (offset < 16)
    {
        return registers[offset];
    }

    if ( offset >= REGISTER_MFG_MODEL && offset < REGISTER_MFG_MODEL + MFG_MODEL_LEN )
    {
        return mfg_model[offset - REGISTER_MFG_MODEL];
    }

    switch(offset)
    {
        case REGISTER_PIN:
            return pin_linear11;

        case REGISTER_PIN+1:
            return pin_linear11 >> 8;

        case REGISTER_VERSION_MAJOR:
            return 0x1;

        case REGISTER_VERSION_MINOR:
            return 0x0;

        case REGISTER_VERSION_PATCH:
            return 0x0;

        default:
            // Unknown offset
            return 0xFF;
    }

}

void I2C_Slave_ReceiveCallback(unsigned int offset, uint8_t data)
{
    if (offset < 16)
    {
        registers[offset] = data;
    }

    // Ignore all other data
}

void I2C_Slave_StartCallback()
{
    // Nothing to do
}

void I2C_Slave_StopCallback()
{
    // Nothing to do
}

// App entry point
void App_Init()
{
    // Init I2C Slave Mode
    I2C_Slave_Init(0x08);
}
