#include "os.h"
#include "app.h"
#include "serial.h"
#include "st7735.h"
#include "charset.h"
#include "pwm.h"
#include "i2c_master.h"

#define RETVAL_OK  0
#define RETVAL_ERR 1

#define FUNCTION_WRITE_DATA   0x10
#define FUNCTION_READ_DATA    0x11
#define FUNCTION_CLEAR_SCREEN 0x20
#define FUNCTION_PRINT_STRING 0x21

#define DATA_CURSOR_X 0x1
#define DATA_CURSOR_Y 0x2

typedef struct {
    uint8_t header;
    uint8_t function;
    uint8_t address;
    uint8_t data_len;
    char    data[];
} Frame_Req;

typedef struct {
    uint8_t status;
    uint8_t len;
    void    *data;
} Frame_Rsp;

void Serial_RxCallback(const char *rx_buffer, int length)
{
    static int posx = 0;
    static int posy = 0;
    
    UNUSED(length);

    Frame_Req *fr = TYPECAST(rx_buffer, Frame_Req*);

    Frame_Rsp rsp = {RETVAL_ERR, 0, NULL_PTR};
    
    switch(fr->function)
    {
        case FUNCTION_CLEAR_SCREEN:
        {
            ST7735_ClearScreen();
            rsp.status = RETVAL_OK;
            break;
        }
            
        case FUNCTION_WRITE_DATA:
        {
            switch(fr->address)
            {
                case DATA_CURSOR_X:
                    posx = READ_PU8(fr->data);
                    rsp.status = RETVAL_OK;
                    break;
                    
                case DATA_CURSOR_Y:
                    posy = READ_PU8(fr->data);
                    rsp.status = RETVAL_OK;
                    break;
            }
            
            break;
        }
        
        case FUNCTION_READ_DATA:
        {
            switch(fr->address)
            {
                case DATA_CURSOR_X:
                    rsp.status = RETVAL_OK;
                    rsp.len = 1;
                    rsp.data = &posx;
                    break;
                    
                case DATA_CURSOR_Y:
                    rsp.status = RETVAL_OK;
                    rsp.len = 1;
                    rsp.data = &posy;
                    break;
            }
            
            break;
        }
            
        case FUNCTION_PRINT_STRING:
        {
            Spi_EnableSlave(Spi_Slave_TFT);
            ST7735_DrawString(posx, posy, fr->data, ST7735_COLOR_BLACK, 1);
            Spi_DisableSlave(Spi_Slave_TFT);
            rsp.status = RETVAL_OK;
            break;
        }
    }
    
    // Send response
    Serial_WriteBytes(&rsp, 2);
    
    // Send additional data
    if (rsp.len > 0 && rsp.data != NULL_PTR)
    {
        Serial_WriteBytes(rsp.data, rsp.len);
    }
}
        
// App entry point
void App_Init()
{
    Serial_Init();
    Spi_Init();
    Spi_Configure(SPI_CLOCK_DIV_2, SPI_MODE_0);
    I2C_Master_Init();

    // Reset screen
    Spi_EnableSlave(Spi_Slave_TFT);
    ST7735_Init();
    ST7735_SetBackgroundColor(ST7735_COLOR_PINK);
    ST7735_ClearScreen();
    Spi_DisableSlave(Spi_Slave_TFT);

    // Turn on screen backlight
    PWM_Init(PWM_5, FALSE);
    PWM_SetPWM(PWM_5, 0x80);

    // Set up tasks
    Os_SetupTask(Timer_MainTask, 1000, &Task_MainCyclic, NULL_PTR);
}

// Main task
Std_ReturnType Task_MainCyclic(void* data)
{
    UNUSED(data);

/*
    static bool state = TRUE;
    if (state) Serial_Println("Tick");
    else Serial_Println("Tock");
    state = !state;
*/
    
    return Status_OK;
}
