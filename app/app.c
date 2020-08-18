#include "os.h"
#include "app.h"
#include "serial.h"
#include "bits.h"
#include "gpio.h"
#include "spi.h"
#include "st7735.h"
#include "hc595.h"
#include "pwm.h"
#include "eeprom.h"
#include "i2c_slave.h"

#include "nyan_cat.xpm"

#define DEVICE_ID_LENGTH 16

typedef struct {
    byte deviceId[DEVICE_ID_LENGTH];
    byte screenInverted;
} DeviceConfig_t;

bool eepromUpdated = FALSE;
bool transmitting = FALSE;

DeviceConfig_t devCfg = {0};

void I2C_Slave_StartCallback()
{
    transmitting = TRUE;
}

void I2C_Slave_StopCallback()
{
    transmitting = FALSE;
}

byte I2C_Slave_TransmitCallback(int offset)
{
    if (offset < DEVICE_ID_LENGTH)
        return devCfg.deviceId[offset];

    if (offset == DEVICE_ID_LENGTH)
        return devCfg.screenInverted;

    return 0xFF;
}

void I2C_Slave_ReceiveCallback(int offset, byte data)
{
    if (offset < DEVICE_ID_LENGTH)
        devCfg.deviceId[offset] = data;

    if (offset == DEVICE_ID_LENGTH)
        devCfg.screenInverted = data;

    eepromUpdated = TRUE;
}

// App entry point
void App_Init()
{
    // Init communication
    I2C_Slave_Init(0x10);
    Serial_Init();
    Spi_Init();

    // Debug dump EEPROM data
    EEPROM_DumpEEPROM(0, 0x20, 16);

    // Read app config from EEPROM
    EEPROM_SyncRead(0, &devCfg, sizeof(DeviceConfig_t));

    // Init TFT
    Spi_Configure(SPI_CLOCK_DIV_2, SPI_MODE_0);
    Spi_EnableSlave(SPI_Slave_TFT);
    ST7735_Init();
    if (!devCfg.screenInverted) ST7735_SetScreenOrientation(ST7735_ScreenOrientation_Landscape);
    else ST7735_SetScreenOrientation(ST7735_ScreenOrientation_Landscape_Inverted);
    ST7735_SetBackgroundColor(ST7735_COLOR_PINK);
    ST7735_ClearScreen();
    ST7735_DrawXPM(nyan_cat, 0, 20, 4);
    ST7735_DrawChars(2, 2, &devCfg.deviceId, DEVICE_ID_LENGTH, ST7735_COLOR_BLACK);
    Spi_DisableSlave(SPI_Slave_TFT);

    // Init HC595
    HC595_Init();

    // Backlight ON
    PWM_Init(PWM_6, FALSE);
    PWM_SetPWM(PWM_6, 0x80);

    // Set up tasks
    Os_SetupTask(Timer_MainTask, 1000, &Task_MainCyclic, NULL_PTR);
}

// Main task
Std_ReturnType Task_MainCyclic(void* data)
{
    UNUSED(data);

    static word out = 1;
    static bool state = TRUE;

    HC595_WriteWord(out++);

    if (state) Serial_PrintLine("Tick");
    else Serial_PrintLine("Tock");
    state = !state;

    if (eepromUpdated && !transmitting)
    {
        EEPROM_SyncWrite(0, &devCfg, sizeof(DeviceConfig_t));
        eepromUpdated = FALSE;
    }

    return Status_OK;
}
