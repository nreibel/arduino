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

typedef struct {
    uint8_t deviceId[DEVICE_ID_LENGTH];
    uint8_t screenInverted;
    uint8_t screenOffsetX;
    uint8_t screenOffsetY;
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

uint8_t I2C_Slave_TransmitCallback(unsigned int offset)
{
    // Read from Device Config
    if ( offset < sizeof(DeviceConfig_t) )
    {
        uint8_t* buf = TYPECAST(&devCfg, uint8_t*);
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

void I2C_Slave_ReceiveCallback(unsigned int offset, uint8_t data)
{
    // Write to Device Config
    if ( offset < sizeof(DeviceConfig_t) )
    {
        uint8_t* buf = TYPECAST(&devCfg, uint8_t*);
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

void Callback_INT0(volatile void *data)
{
    UNUSED(data);
    Serial_PrintLine("INT0 !!!");
}

void Callback_INT1(volatile void *data)
{
    UNUSED(data);
    Serial_PrintLine("INT1 !!!");
}

// App entry point
void App_Init()
{
    // Init communication
    I2C_Master_Init();
    Serial_Init();
    Spi_Init();

    // Enable external interrupts
    GPIO_EnableInterrupt(GPIO_INT0, GPIO_Edge_Rising, &Callback_INT0, NULL_PTR);
    GPIO_EnableInterrupt(GPIO_INT1, GPIO_Edge_Rising, &Callback_INT1, NULL_PTR);

    // Init accelerometer
    MMA8452Q_Init();
    MMA8452Q_SetRange(MMA8452Q_Range_8G, FALSE);
    MMA8452Q_SetTransientMode(TRUE, TRUE, TRUE, TRUE);
    MMA8452Q_SetTransientThreshold(1.5);
    MMA8452Q_EnableInterrupts( BIT(5) );
    MMA8452Q_SetStandby(FALSE);

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
        ST7735_CalibrationData.Offset_X = 1;
    }

    if (devCfg.screenOffsetY != 0xFF)
    {
        ST7735_CalibrationData.Offset_Y = devCfg.screenOffsetY;
    }
    else
    {
        ST7735_CalibrationData.Offset_Y = 2;
    }

    if (devCfg.screenInverted != 0xFF)
    {
        ST7735_CalibrationData.InvertScreen = devCfg.screenInverted;
    }

    // Init TFT
    Spi_Configure(SPI_CLOCK_DIV_2, SPI_MODE_0);
    Spi_EnableSlave(SPI_Slave_TFT);

    ST7735_Init();
    ST7735_SetBackgroundColor(ST7735_COLOR_PINK);
    ST7735_ClearScreen();

    ST7735_DrawXPM(nyan_cat, 0, 30, 4);
    ST7735_DrawChars(2, 2, &devCfg.deviceId, DEVICE_ID_LENGTH, ST7735_COLOR_BLACK);

    Spi_DisableSlave(SPI_Slave_TFT);

    // Init HC595
    HC595_Init();

    // Backlight ON
    PWM_Init(PWM_6, FALSE);
    PWM_SetPWM(PWM_6, 0x80);

    // Set up tasks
//     Os_SetupTask(Timer_MainTask, 1000, &Task_MainCyclic, NULL_PTR);
    Os_SetupTask(Timer_Accelerometer, 200, &Task_Accelerometer, NULL_PTR);
}

// Accelerometer
Std_ReturnType Task_Accelerometer(void* data)
{
    UNUSED(data);

    static char str[32];

    uint8_t status = 0;
    uint8_t interrupt_status = 0;
    uint8_t transient_status = 0;
    MMA8452Q_Data_t acc_data = {0};

    MMA8452Q_GetStatus(&status);
    MMA8452Q_GetInterruptStatus(&interrupt_status);
    MMA8452Q_GetTransientSource(&transient_status);
    MMA8452Q_GetData(&acc_data);

    // Print raw data
    Serial_PrintLine("--");

    sprintf(str, "Status = %02x", status );
    Serial_PrintLine(str);

    sprintf(str, "Interrupts = %02x", interrupt_status );
    Serial_PrintLine(str);

    sprintf(str, "Transient = %02x", transient_status );
    Serial_PrintLine(str);

    sprintf(str, "Acc X = %4d", acc_data.acc_x);
    Serial_PrintLine(str);

    sprintf(str, "Acc Y = %4d", acc_data.acc_y);
    Serial_PrintLine(str);

    sprintf(str, "Acc Z = %4d", acc_data.acc_z);
    Serial_PrintLine(str);

    return Status_OK;
}

// Main task
Std_ReturnType Task_MainCyclic(void* data)
{
    UNUSED(data);

    static uint16_t out = 1;
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
