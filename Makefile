# Serial
SERIAL_TTY := /dev/ttyUSB0
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
LDFLAGS := -u,vfprintf
LIBS := printf_flt m

# Enable selected modules
MODULES := os gpio serial timer icp

include $(ARDUINO_LIBS)/makefile.include
