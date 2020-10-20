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
#include "max7219.h"
#include "crc.h"
#include "string.h"
#include "stdio.h"

#if 0
#include "nyan_cat.xpm"
#endif

#define DEVICE_ID_LENGTH 16

uint8_t icon_8x8[8] = {0x00, 0x3C, 0x42, 0x5A, 0x52, 0x42, 0x3C, 0x00};

typedef struct {
    uint8_t deviceId[DEVICE_ID_LENGTH];
} DeviceConfig_t;
DeviceConfig_t deviceConfig = {0};

bool eepromReset = FALSE;
bool eepromUpdated = FALSE;
bool transmitting = FALSE;


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
        return UINT8_PTR(&deviceConfig)[offset];

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
        UINT8_PTR(&deviceConfig)[offset] = data;
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
    Serial_Println("INT0 !!!");
}

void Callback_INT1(volatile void *data)
{
    UNUSED(data);
    Serial_Println("INT1 !!!");
}

// App entry point
void App_Init()
{
    // Init communication
    I2C_Master_Init();
    Serial_Init();
    Spi_Init();

    // Compute CRC
    uint32_t crc = 0;
    CRC32_Init(&crc);
    CRC32_Update('t', &crc);
    CRC32_Update('e', &crc);
    CRC32_Update('s', &crc);
    CRC32_Update('t', &crc);
    CRC32_Final(&crc);

    char buffer[32];
    sprintf(buffer, "CRC is 0x%08lx", crc);
    Serial_Println(buffer);

    // Enable external interrupts
    GPIO_EnableInterrupt(GPIO_INT0, GPIO_Edge_Rising, &Callback_INT0, NULL_PTR);
    GPIO_EnableInterrupt(GPIO_INT1, GPIO_Edge_Rising, &Callback_INT1, NULL_PTR);

#if 0
    // MAX7219 LED Matrix
    Spi_Configure(SPI_CLOCK_DIV_2, SPI_MODE_0);
    MAX7219_Init(Spi_Slave_LED);

    Os_SetupTask(Timer_LedMatrix, 150, &Task_LedMatrix, NULL_PTR);
#endif

#if 0
    // Init accelerometer
    MMA8452Q_Init();
    MMA8452Q_SetRange(MMA8452Q_Range_2G, FALSE);

    // Detect shocks
    MMA8452Q_SetTransientMode(TRUE, TRUE, TRUE, TRUE);
    MMA8452Q_SetTransientThreshold(1.5);
    MMA8452Q_SetInterruptConfig(MMA8452Q_InterruptSource_Transient, MMA8452Q_InterruptPin_INT1);
    MMA8452Q_EnableInterrupt(MMA8452Q_InterruptSource_Transient);

    // Detect freefall
    MMA8452Q_SetFreefallMode(TRUE, TRUE, TRUE, TRUE);
    MMA8452Q_SetFreefallThreshold(0.8);
    MMA8452Q_SetInterruptConfig(MMA8452Q_InterruptSource_Freefall, MMA8452Q_InterruptPin_INT2);
    MMA8452Q_EnableInterrupt(MMA8452Q_InterruptSource_Freefall);

    MMA8452Q_SetStandby(FALSE);

    Os_SetupTask(Timer_Accelerometer, 200, &Task_Accelerometer, NULL_PTR);
#endif

    // Debug dump EEPROM data over Serial
    EEPROM_DumpEEPROM(0, 0x20, 16);

    // Read app config from EEPROM
    EEPROM_SyncRead(0, &deviceConfig, sizeof(DeviceConfig_t));

#if 0
    // Init TFT
    Spi_Configure(SPI_CLOCK_DIV_2, SPI_MODE_0);
    Spi_EnableSlave(SPI_Slave_TFT);

    ST7735_Init();
    ST7735_SetBackgroundColor(ST7735_COLOR_PINK);
    ST7735_ClearScreen();

    ST7735_DrawXPM(nyan_cat, 0, 30, 4);
    ST7735_DrawChars(2, 2, &deviceConfig.deviceId, DEVICE_ID_LENGTH, ST7735_COLOR_BLACK);

    Spi_DisableSlave(SPI_Slave_TFT);

    // Init HC595
    HC595_Init();

    // Backlight ON
    PWM_Init(PWM_6, FALSE);
    PWM_SetPWM(PWM_6, 0x80);
#endif

    // Set up tasks
    Os_SetupTask(Timer_MainTask, 1000, &Task_MainCyclic, NULL_PTR);
}

Std_ReturnType Task_LedMatrix(void* data)
{
    UNUSED(data);

    static uint8_t offset = 0;

    for(int digit = 0 ; digit < 8 ; digit++)
    {
        uint8_t col = (digit + offset) % 8;
        MAX7219_SetDigit(Spi_Slave_LED, digit, icon_8x8[col]);
    }

    offset++;

    return Status_OK;
}

// Accelerometer
Std_ReturnType Task_Accelerometer(void* data)
{
    UNUSED(data);

    static char str[32];

    uint8_t status = 0;
    uint8_t interrupt_status = 0;
    uint8_t transient_status = 0;
    uint8_t freefall_status = 0;
    MMA8452Q_Data_t acc_data = {0};

    MMA8452Q_GetStatus(&status);
    MMA8452Q_GetInterruptStatus(&interrupt_status);
    MMA8452Q_GetTransientSource(&transient_status);
    MMA8452Q_GetFreefallSource(&freefall_status);
    MMA8452Q_GetData(&acc_data);

    // Print raw data
    Serial_Println("--");

    sprintf(str, "Status = %02x", status );
    Serial_Println(str);

    if ( IS_SET_BIT(interrupt_status, MMA8452Q_InterruptSource_Freefall) )
        Serial_Println("Freefall detected!");
    else
        Serial_Println("No freefall");

    if ( IS_SET_BIT(interrupt_status, MMA8452Q_InterruptSource_Transient) )
        Serial_Println("Shock detected!");
    else
        Serial_Println("No shock");

    sprintf(str, "Transient = %02x", transient_status );
    Serial_Println(str);

    sprintf(str, "Freefall = %02x", freefall_status );
    Serial_Println(str);

    sprintf(str, "Acc X = %4d", acc_data.acc_x);
    Serial_Println(str);

    sprintf(str, "Acc Y = %4d", acc_data.acc_y);
    Serial_Println(str);

    sprintf(str, "Acc Z = %4d", acc_data.acc_z);
    Serial_Println(str);

    return Status_OK;
}

// Main task
Std_ReturnType Task_MainCyclic(void* data)
{
    UNUSED(data);

    static uint16_t out = 1;
    static bool state = TRUE;

    HC595_WriteWord(out++);

    if (state) Serial_Println("Tick");
    else Serial_Println("Tock");
    state = !state;

    if (eepromUpdated && !transmitting)
    {
        if (eepromReset)
        {
            memset(&deviceConfig, 0, sizeof(DeviceConfig_t));
            eepromReset = FALSE;
        }

        EEPROM_SyncWrite(0, &deviceConfig, sizeof(DeviceConfig_t));
        eepromUpdated = FALSE;
    }

    return Status_OK;
}
