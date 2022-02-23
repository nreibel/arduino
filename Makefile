# Serial
SERIAL_TTY := /dev/ttyACM0
SERIAL_BAUDRATE := 19200

ARDUINO_LIBS := .

TARGET := $(ARDUINO_LIBS)/targets/leonardo.mk

APP := out

# Libraries paths
SRC_DIR := $(ARDUINO_LIBS)/src
CFG_DIR := ./cfg

# Source and header files
INCLUDES := ./app ./res
SOURCES := app/app.c

# Enable support for doubles and floats in printf
# Adds ~2.5kB to ROM data!!!
LDFLAGS := -u,vfprintf
LIBS := printf_flt m

# Enable selected modules
MODULES := os timer serial gpio

include $(ARDUINO_LIBS)/makefile.include
