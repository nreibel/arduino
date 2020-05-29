#include "port.h"
#include "spi_cfg.h"
#include "gpio_uno.h"

GPIO Spi_MOSI = GPIO_MOSI;
GPIO Spi_MISO = GPIO_ISO;
GPIO Spi_SCK  = GPIO_SCK;

GPIO SlaveSelect_Pins[SPI_NUMBER_OF_SLAVES] =
{
    GPIO_10, // SPI_Slave_TFT
};
