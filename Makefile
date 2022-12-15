# Serial
SERIAL_TTY := /dev/ttyACM0
SERIAL_BAUDRATE := 19200

ARDUINO_LIBS := .

TARGET := $(ARDUINO_LIBS)/targets/uno.mk

APP := out

# Libraries paths
SRC_DIR := $(ARDUINO_LIBS)/src
CFG_DIR := ./cfg

# Source and header files
INCLUDES := ./app ./res
SOURCES := app/app.c

# Enable support for doubles and floats in printf
# Adds ~2.5kB to ROM data!!!
# LDFLAGS := -u,vfprintf
# LIBS := printf_flt m

# Enable selected modules
MODULES := os gpio timer serial i2c crc

# Use only LL API to save on memory
GPIO_LL=1
# SERIAL_LL=1
TIMER_LL=1
SPI_LL=1
I2C_LL=1
ADC_LL=1
EEPROM_LL=1

# Must be last
include $(ARDUINO_LIBS)/makefile.include
