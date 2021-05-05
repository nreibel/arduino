#include "os.h"
#include "app.h"
#include "serial.h"
#include "serial_tp.h"

#define FUNCTION_WRITE_DATA 0x10
#define FUNCTION_READ_DATA  0x11

#define DATA_X 0x01
#define DATA_Y 0x02

void Serial_TP_Callback(Serial_TP_Request *req, Serial_TP_Response *rsp)
{
    static uint8_t data_x = 0;
    static uint8_t data_y = 0;

    switch(req->function)
    {
        case FUNCTION_READ_DATA:
        {
            switch(req->address)
            {
                case DATA_X:
                    rsp->length = 1;
                    rsp->data[0] = data_x;
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;

                case DATA_Y:
                    rsp->length = 1;
                    rsp->data[0] = data_y;
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;

                default:
                    rsp->status = SERIAL_TP_RETCODE_ADDRESS_INVALID;
                    break;
            }
            break;
        }

        case FUNCTION_WRITE_DATA:
        {
            switch(req->address)
            {
                case DATA_X:
                    data_x = req->data[0];
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;

                case DATA_Y:
                    data_y = req->data[0];
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;

                default:
                    rsp->status = SERIAL_TP_RETCODE_ADDRESS_INVALID;
                    break;
            }
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
    Serial_Init();
    Serial_Print("READY");
}
