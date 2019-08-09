#include "port.h"

PinDef Spi_MOSI = {Port_B, Pin_3};
PinDef Spi_MISO = {Port_B, Pin_4};
PinDef Spi_SCK = {Port_B, Pin_5};

PinDef SlaveSelect_Pins[] =
{
    {Port_B, 0}, // SPI_Slave_TFT
};
