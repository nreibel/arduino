#include "spi_cfg.h"
#include "gpio.h"

GPIO Spi_SCK  = D13;
GPIO Spi_MISO = D12;
GPIO Spi_MOSI = D11;

GPIO SlaveSelect_Pins[SPI_NUMBER_OF_SLAVES] =
{
    D10, // SPI_Slave_TFT
};
