#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "eeprom.h"

// App entry point
void App_Init()
{
    // Init buses
    serial_bus_init(SERIAL_BUS_0, 19200);

    eeprom_init();

    char str_buffer[32];
    uint8_t eeprom_buffer[16];
    uint16_t eeprom_addr = 0;

    for (int line = 0; line < 16; line++)
    {
        eeprom_read(eeprom_addr, eeprom_buffer, 16);

        sprintf(str_buffer, "0x%04X | ", eeprom_addr);
        serial_print(SERIAL_BUS_0, str_buffer);

        for (int c = 0 ; c < 16 ; c++)
        {
            sprintf(str_buffer, "0x%02X ", eeprom_buffer[c]);
            serial_print(SERIAL_BUS_0, str_buffer);
        }

        serial_print(SERIAL_BUS_0, "\r\n");

        eeprom_addr += 16;
    }


    // Init tasks
    Os_SetupTask(Timer_MainTask, 500, Task_Main, NULL_PTR);

    // Ready!
    serial_println(SERIAL_BUS_0, "READY");
}

// Main task
Std_ReturnType Task_Main(void* data)
{
    UNUSED(data);

    return Status_OK;
}
