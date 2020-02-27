#include "port.h"
#include "gpio_uno.h"

PinDef Spi_MOSI = GPIO_11;
PinDef Spi_MISO = GPIO_12;
PinDef Spi_SCK = GPIO_13;

PinDef SlaveSelect_Pins[] =
{
    GPIO_10, // SPI_Slave_TFT
};
