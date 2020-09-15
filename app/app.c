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
#include "i2c_master.h"
#include "mma8452q.h"
#include "string.h"
#include "stdio.h"

#include "nyan_cat.xpm"

#define DEVICE_ID_LENGTH 16

// TODO : cleanup
#define RANGE(x, in_min, in_max, out_min, out_max) (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min

typedef struct {
    byte deviceId[DEVICE_ID_LENGTH];
    byte screenInverted;
    byte screenOffsetX;
    byte screenOffsetY;
} DeviceConfig_t;

bool eepromReset = FALSE;
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

byte I2C_Slave_TransmitCallback(unsigned int offset)
{
    // Read from Device Config
    if ( offset < sizeof(DeviceConfig_t) )
    {
        byte* buf = TYPECAST(&devCfg, byte*);
        return buf[offset];
    }

    // EEPROM Reset address
    if (offset == 0xFE)
        return 0;

    // Device Reset address
    if (offset == 0xFF)
        return 0;

    return 0xFF;
}

void I2C_Slave_ReceiveCallback(unsigned int offset, byte data)
{
    // Write to Device Config
    if ( offset < sizeof(DeviceConfig_t) )
    {
        byte* buf = TYPECAST(&devCfg, byte*);
        buf[offset] = data;
        eepromUpdated = TRUE;
    }

    // EEPROM Reset address
    if (offset == 0xFE && data == 0x01)
    {
        eepromReset = TRUE;
        eepromUpdated = TRUE;
    }

    // Device Reset address
    if (offset == 0xFF && data == 0x01)
        Os_HardReset();
}

// App entry point
void App_Init()
{
    // Init communication
    I2C_Master_Init();
    Serial_Init();
    Spi_Init();

    // Init accelerometer
    MMA8452Q_Init();

    // Debug dump EEPROM data over Serial
    EEPROM_DumpEEPROM(0, 0x20, 16);

    // Read app config from EEPROM
    EEPROM_SyncRead(0, &devCfg, sizeof(DeviceConfig_t));

    // Read calibration data
    if (devCfg.screenOffsetX != 0xFF)
    {
        ST7735_CalibrationData.Offset_X = devCfg.screenOffsetX;
    }
    else
    {
        ST7735_CalibrationData.Offset_X = 2;
    }

    if (devCfg.screenOffsetY != 0xFF)
    {
        ST7735_CalibrationData.Offset_Y = devCfg.screenOffsetY;
    }
    else
    {
        ST7735_CalibrationData.Offset_Y = 1;
    }

    if (devCfg.screenInverted != 0xFF)
    {
        ST7735_CalibrationData.InvertScreen = devCfg.screenInverted;
    }

    // Init TFT
    Spi_Configure(SPI_CLOCK_DIV_2, SPI_MODE_0);
    Spi_EnableSlave(SPI_Slave_TFT);
    ST7735_Init();
    ST7735_SetBackgroundColor(ST7735_COLOR_BLACK);
    ST7735_ClearScreen();
    ST7735_DrawXPM(nyan_cat, 0, 0, 1);
    ST7735_DrawChars(2, 2, &devCfg.deviceId, DEVICE_ID_LENGTH, ST7735_COLOR_BLACK);
    Spi_DisableSlave(SPI_Slave_TFT);

    // Init HC595
    HC595_Init();

    // Backlight ON
    PWM_Init(PWM_6, FALSE);
    PWM_SetPWM(PWM_6, 0x80);

    // Set up tasks
    Os_SetupTask(Timer_MainTask, 1000, &Task_MainCyclic, NULL_PTR);
    Os_SetupTask(Timer_Accelerometer, 20, &Task_Accelerometer, NULL_PTR);
}

// Accelerometer
Std_ReturnType Task_Accelerometer(void* data)
{
    UNUSED(data);

    // static char str[32];
    static int posx = 64;
    static int posy = 80;

    MMA8452Q_Data_t acc_data = {0};
    MMA8452Q_Read(&acc_data);

    // Offset for flat sensor position
    acc_data.acc_x -= 128;
    acc_data.acc_y -= 128;

    // sprintf(str, "Status = %02x", acc_data.status );
    // Serial_PrintLine(str);

    // sprintf(str, "Acc X = %3d", acc_data.acc_x);
    // Serial_PrintLine(str);

    // sprintf(str, "Acc Y = %3d", acc_data.acc_y);
    // Serial_PrintLine(str);

    // sprintf(str, "Acc Z = %3d", acc_data.acc_z);
    // Serial_PrintLine(str);

    Spi_EnableSlave(SPI_Slave_TFT);

    // Erase previous dot
    ST7735_FillRectangle(posx, posy, 5, 5, ST7735_COLOR_WHITE);

    // Get new dot position
    int new_posx = RANGE(acc_data.acc_x, 192, 64, 0, 128);
    int new_posy = RANGE(acc_data.acc_y, 64, 192, 0, 160);

    // Smooth data
    posx = (new_posx + 7*posx)/8;
    posy = (new_posy + 7*posy)/8;

    // Draw new dot
    ST7735_FillRectangle(posx, posy, 5, 5, ST7735_COLOR_FUSCHIA);
    Spi_DisableSlave(SPI_Slave_TFT);

    return Status_OK;
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
        if (eepromReset)
        {
            memset(&devCfg, 0, sizeof(DeviceConfig_t));
            eepromReset = FALSE;
        }

        EEPROM_SyncWrite(0, &devCfg, sizeof(DeviceConfig_t));
        eepromUpdated = FALSE;
    }

    return Status_OK;
}
