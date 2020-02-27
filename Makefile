SHELL=/bin/sh

# SRC_DIR=src/
# CFG_DIR=cfg/
# TARGET=targets/uno.mk
# MODULES=eeprom hc595 i2c keys lcd math os port pwm serial spi st7735 stack timer uss
# APP_DIR=app/
# INCLUDES=$(APP_DIR)/api $(APP_DIR)/cfg \
# app: $(APP_DIR)/src/app.o $(APP_DIR)/cfg/app_cfg.o

# Path to build tools
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
UPLOAD=avrdude
TERM=konsole
MONITOR=picocom

# Output files
OBJ=obj
OUT=bin
FNAME=out-$(ARCH)

# Compilation options and flags
WARNINGS+=all extra undef
CFLAGS+=-O3 -g0 -mmcu=$(ARCH) -ffunction-sections -fdata-sections
LDFLAGS+=-Wl,-gc-sections -Wl,--relax

# Serial monitor
BAUD_RATE=19200

include $(TARGET)
include $(MODULES:%=$(SRC_DIR)/%/makefile.include)

all: clean prepare hex

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
	@$(TERM) -e "$(MONITOR) -b $(BAUD_RATE) $(SERIAL_TTY)"

stop:
	@for pid in `pgrep $(MONITOR)`; do kill $$pid; done
