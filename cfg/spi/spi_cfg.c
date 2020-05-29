#include "port.h"
#include "gpio_uno.h"

GPIO Spi_MOSI = GPIO_11;
GPIO Spi_MISO = GPIO_12;
GPIO Spi_SCK = GPIO_13;

GPIO SlaveSelect_Pins[] =
{
    GPIO_10, // SPI_Slave_TFT
};
