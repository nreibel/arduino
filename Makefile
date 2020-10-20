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

# Compilation flags
# LDFLAGS = -u,vfprintf
# LIBS = printf_flt m

MODULES := os gpio serial spi crc st7735 pwm hc595 eeprom i2c_master mma8452q max7219
app: app/app.o

include $(ARDUINO_LIBS)/makefile.include
