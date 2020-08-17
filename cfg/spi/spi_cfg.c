#include "port.h"
#include "spi_cfg.h"
#include "gpio_uno.h"

Port_GPIO Spi_MOSI = GPIO_MOSI;
Port_GPIO Spi_MISO = GPIO_MISO;
Port_GPIO Spi_SCK  = GPIO_SCK;

Port_GPIO SlaveSelect_Pins[SPI_NUMBER_OF_SLAVES] =
{
    GPIO_10, // SPI_Slave_TFT
};
