# Serial
SERIAL_TTY := /dev/ttyUSB0
SERIAL_BAUDRATE := 19200

ARDUINO_LIBS := ./

# Libraries paths
SRC_DIR := $(ARDUINO_LIBS)/src
TARGET := $(ARDUINO_LIBS)/targets/uno.mk

# Include paths
CFG_DIR := ./cfg
INCLUDES := ./app ./res

# Enable support for doubles and floats in printf
# Adds ~2.5kB to ROM data!!!
LDFLAGS = -u,vfprintf
LIBS = printf_flt m

# TODO : handle modules dependencies
MODULES := os serial eeprom
app: app/app.o

include $(ARDUINO_LIBS)/makefile.include
