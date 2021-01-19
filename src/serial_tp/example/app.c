#include "os.h"
#include "serial.h"
#include "serial_tp.h"

#define FUNCTION_WRITE_DATA   0x10
#define FUNCTION_READ_DATA    0x11

#define DATA_X 0x01
#define DATA_Y 0x02

void Serial_TP_Callback(Serial_TP_Request *req, Serial_TP_Response *rsp)
{
    static int x = 0;
    static int y = 0;

    switch(req->function)
    {
        case FUNCTION_WRITE_DATA:
        {
            switch(req->address)
            {
                case DATA_X:
                {
                    x = READ_PU8(req->data);
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;
                }

                case DATA_Y:
                {
                    y = READ_PU8(req->data);
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

        case FUNCTION_READ_DATA:
        {
            switch(req->address)
            {
                case DATA_X:
                {
                    rsp->data_len = 1;
                    rsp->data = &x;
                    rsp->status = SERIAL_TP_RETCODE_OK;
                    break;
                }

                case DATA_Y:
                {
                    rsp->data_len = 1;
                    rsp->data = &y;
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
    Serial_TP_Init();

    // Not mandatory, useful for client app to detect device
    Serial_Print("READY");
}
