#include "os.h"
#include "app.h"
#include "serial.h"
#include "serial_tp.h"
#include "st7735.h"
#include "charset.h"
#include "pwm.h"
#include "i2c_master.h"

#define FUNCTION_WRITE_DATA   0x10
#define FUNCTION_READ_DATA    0x11
#define FUNCTION_CLEAR_SCREEN 0x20
#define FUNCTION_PRINT_STRING 0x21

#define DATA_BACKLIGHT 0x00
#define DATA_CURSOR_X  0x01
#define DATA_CURSOR_Y  0x02
#define DATA_BGCOLOR   0x03
#define DATA_FGCOLOR   0x04


void Serial_TP_Callback(Serial_TP_Request *req, Serial_TP_Response *rsp)
{
    static int posx = 0;
    static int posy = 0;
    static st7735_color_t fgcolor = ST7735_COLOR_BLACK;

    switch(req->function)
    {
        case FUNCTION_CLEAR_SCREEN:
        {
            Spi_EnableSlave(Spi_Slave_TFT);
            ST7735_ClearScreen();
            Spi_DisableSlave(Spi_Slave_TFT);

            rsp->status = SERIAL_TP_RETCODE_OK;
            break;
        }

        case FUNCTION_WRITE_DATA:
        {
            switch(req->address)
            {
                case DATA_CURSOR_X:
                {
                    posx = READ_PU8(req->data);
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;
                }

                case DATA_CURSOR_Y:
                {
                    posy = READ_PU8(req->data);
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;
                }

                case DATA_BGCOLOR:
                {
                    if (req->data_len != 1)
                    {
                        rsp->status = SERIAL_TP_RETCODE_DATA_INVALID;
                    }
                    else switch( READ_PU8(req->data) )
                    {
                        case 0x00:
                        {
                                ST7735_SetBackgroundColor(ST7735_COLOR_BLACK);
                                rsp->status = SERIAL_TP_RETCODE_OK;
                                break;
                        }

                        case 0x01:
                        {
                                ST7735_SetBackgroundColor(ST7735_COLOR_WHITE);
                                rsp->status = SERIAL_TP_RETCODE_OK;
                                break;
                        }

                        default:
                        {
                                rsp->status = SERIAL_TP_RETCODE_DATA_INVALID;
                                break;
                        }
                    }

                    break;
                }

                case DATA_FGCOLOR:
                {
                    if (req->data_len != 1)
                    {
                        rsp->status = SERIAL_TP_RETCODE_DATA_INVALID;
                    }
                    else switch( READ_PU8(req->data) )
                    {
                        case 0x00:
                        {
                                fgcolor = ST7735_COLOR_BLACK;
                                rsp->status = SERIAL_TP_RETCODE_OK;
                                break;
                        }

                        case 0x01:
                        {
                                fgcolor = ST7735_COLOR_WHITE;
                                rsp->status = SERIAL_TP_RETCODE_OK;
                                break;
                        }

                        default:
                        {
                                rsp->status = SERIAL_TP_RETCODE_DATA_INVALID;
                                break;
                        }
                    }

                    break;
                }

                case DATA_BACKLIGHT:
                {
                    PWM_SetPWM(PWM_5, 0xFF - READ_PU8(req->data) );
                    break;
                }

                default:
                {
                    rsp->status = SERIAL_TP_RETCODE_ADDRESS_INVALID;
                    break;
                }
            }

            break;
        }

        case FUNCTION_READ_DATA:
        {
            switch(req->address)
            {
                case DATA_CURSOR_X:
                {
                    rsp->data_len = 1;
                    rsp->data = &posx;
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;
                }

                case DATA_CURSOR_Y:
                {
                    rsp->data_len = 1;
                    rsp->data = &posy;
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;
                }

                default:
                {
                    rsp->status = SERIAL_TP_RETCODE_ADDRESS_INVALID;
                    break;
                }
            }

            break;
        }

        case FUNCTION_PRINT_STRING:
        {
            Spi_EnableSlave(Spi_Slave_TFT);
            ST7735_DrawString(posx, posy, TYPECAST(req->data, char*), fgcolor, 1);
            Spi_DisableSlave(Spi_Slave_TFT);

            rsp->status = SERIAL_TP_RETCODE_OK;
            break;
        }

        default:
        {
            rsp->status = SERIAL_TP_RETCODE_FUNCTION_INVALID;
            break;
        }
    }
}
        
// App entry point
void App_Init()
{
    // Turn off screen backlight
    PWM_Init(PWM_5, TRUE);
    PWM_SetPWM(PWM_5, 0xFF);

    // Init SPI
    Spi_Init();
    Spi_Configure(SPI_CLOCK_DIV_2, SPI_MODE_0);

    // Init I2C Master Mode
    I2C_Master_Init();

    // Init Serial TP
    Serial_Init();
    Serial_TP_Init();

    // Reset screen
    Spi_EnableSlave(Spi_Slave_TFT);
    ST7735_Init();
    ST7735_SetBackgroundColor(ST7735_COLOR_FUSCHIA);
    ST7735_ClearScreen();
    Spi_DisableSlave(Spi_Slave_TFT);

    // Turn on screen backlight
    PWM_Init(PWM_5, TRUE);
    PWM_SetPWM(PWM_5, 0x80);

    // Set up tasks
    Os_SetupTask(Timer_MainTask, 1000, &Task_MainCyclic, NULL_PTR);

    Serial_Print("READY");
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
