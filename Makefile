SHELL=/bin/sh

# Path to build tools
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
UPLOAD=avrdude
TERM=konsole -e

# Output files
OBJ=obj
OUT=bin
FNAME=out-$(ARCH)
SRC_DIR=src/
APP_DIR=app/

# Compilation options and flags
WARNINGS=all extra undef
CFLAGS=-O3 -g0 -mmcu=$(ARCH) -ffunction-sections -fdata-sections
LDFLAGS=-Wl,-gc-sections -Wl,--relax

# Serial monitor
BAUD_RATE=19200

MODULES=eeprom hc595 i2c keys lcd math os port pwm serial spi st7735 stack timer uss

INCLUDES=\
	$(APP_DIR)/api \
	$(APP_DIR)/cfg \

include target_$(TARGET).mk
include $(MODULES:%=src/%/makefile.include)

all: clean prepare hex

app:    $(APP_DIR)/src/app.o \
	$(APP_DIR)/cfg/app_cfg.o

hex: app $(MODULES)
	@echo "Linking object files..."
	@$(CC) $(LDFLAGS) -Wl,-Map,$(OUT)/$(FNAME).map $(CFLAGS) $(OBJ)/$(ARCH)/*.o -o $(OUT)/$(FNAME).elf
	@echo "Creating HEX file..."
	@$(OBJCOPY) -O ihex $(OUT)/$(FNAME).elf $(OUT)/$(FNAME).hex
	@echo "=================================="
	@$(SIZE) -C --mcu=$(ARCH) $(OUT)/$(FNAME).elf
	@echo "Done"

clean:
	@echo "Cleaning workspace"
	@rm -Rf $(OBJ)
	@rm -Rf $(OUT)

prepare:
	@echo "Preparing workspace"
	@mkdir -p $(OBJ)/$(ARCH) $(OUT)

upload: stop
	$(UPLOAD) -b $(UPLOAD_BAUD_RATE) -c $(PROGRAMMER) -p $(ARCH) -P $(SERIAL_TTY) -U flash:w:$(OUT)/$(FNAME).hex

# Generic rules for compiling objects
%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(addprefix -I,$(INCLUDES)) $(addprefix -W,$(WARNINGS)) $(CFLAGS) -c -o $(OBJ)/$(ARCH)/$(@F) $<

# Serial monitor
monitor: stop
	@$(TERM) "picocom -b $(BAUD_RATE) $(SERIAL_TTY)" &
