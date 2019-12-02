SHELL=/bin/sh

ARCH=atmega328p
# ARCH=atmega32u4

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

ifeq ($(ARCH),atmega328p)
	PROGRAMMER=arduino
	UPLOAD_BAUD_RATE=115200
endif

ifeq ($(ARCH),atmega32u4)
	PROGRAMMER=avr109
	UPLOAD_BAUD_RATE=57600
endif

# Compilation options and flags
WARNINGS=all extra undef
CFLAGS=-O3 -g0 -mmcu=$(ARCH) -ffunction-sections -fdata-sections
LDFLAGS=-Wl,-gc-sections -Wl,--relax

# Serial config
SERIAL_TTY=/dev/ttyACM0
SERIAL_MONITOR=screen
SERIAL_BAUD_RATE=19200

INCLUDES=\
	$(SRC_DIR)/app/api \
	$(SRC_DIR)/app/cfg \
	$(SRC_DIR)/eeprom/api \
	$(SRC_DIR)/eeprom/cfg \
	$(SRC_DIR)/i2c/api \
	$(SRC_DIR)/i2c/cfg \
	$(SRC_DIR)/keys/api \
	$(SRC_DIR)/keys/cfg \
	$(SRC_DIR)/lcd/api \
	$(SRC_DIR)/lcd/cfg \
	$(SRC_DIR)/math/api \
	$(SRC_DIR)/math/cfg \
	$(SRC_DIR)/os/api \
	$(SRC_DIR)/os/cfg \
	$(SRC_DIR)/port/api \
	$(SRC_DIR)/port/cfg \
	$(SRC_DIR)/pwm/api \
	$(SRC_DIR)/pwm/cfg \
	$(SRC_DIR)/serial/api \
	$(SRC_DIR)/serial/cfg \
	$(SRC_DIR)/spi/api \
	$(SRC_DIR)/spi/cfg \
	$(SRC_DIR)/st7735/api \
	$(SRC_DIR)/st7735/cfg \
	$(SRC_DIR)/stack/api \
	$(SRC_DIR)/stack/cfg \
	$(SRC_DIR)/timer/api \
	$(SRC_DIR)/timer/cfg \
	$(SRC_DIR)/uss/api \
	$(SRC_DIR)/uss/cfg \

.PHONY: all modules hex clean prepare upload monitor stop

all: clean prepare hex

modules: eeprom i2c keys lcd math os port pwm serial spi st7735 stack timer uss

hex: app modules
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

# Each software component must be created here
app:    $(SRC_DIR)/app/src/app.o $(SRC_DIR)/app/cfg/app_cfg.o
eeprom: $(SRC_DIR)/eeprom/src/eeprom.o
i2c:    $(SRC_DIR)/i2c/src/i2c.o $(SRC_DIR)/i2c/cfg/i2c_cfg.o
keys:   $(SRC_DIR)/keys/src/keys.o $(SRC_DIR)/keys/cfg/keys_cfg.o
lcd:    $(SRC_DIR)/lcd/src/lcd.o
math:   $(SRC_DIR)/math/src/math.o
os:     $(SRC_DIR)/os/src/os.o $(SRC_DIR)/os/src/os_$(ARCH).o $(SRC_DIR)/os/src/utils.o $(SRC_DIR)/os/cfg/os_cfg.o
port:   $(SRC_DIR)/port/src/port.o $(SRC_DIR)/port/cfg/port_cfg.o
pwm:    $(SRC_DIR)/pwm/src/pwm.o $(SRC_DIR)/pwm/cfg/pwm_cfg.o
serial: $(SRC_DIR)/serial/src/serial.o $(SRC_DIR)/serial/src/serial_$(ARCH).o $(SRC_DIR)/serial/src/serial_rx_async.o $(SRC_DIR)/serial/src/serial_tx_async.o
spi:    $(SRC_DIR)/spi/src/spi.o $(SRC_DIR)/spi/src/spi_$(ARCH).o $(SRC_DIR)/spi/cfg/spi_cfg.o
st7735: $(SRC_DIR)/st7735/src/st7735.o $(SRC_DIR)/st7735/cfg/st7735_cfg.o
stack:  $(SRC_DIR)/stack/src/stack.o
timer:  $(SRC_DIR)/timer/src/timer.o
uss:    $(SRC_DIR)/uss/src/uss.o $(SRC_DIR)/uss/cfg/uss_cfg.o


# Generic rules for compiling objects
%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(addprefix -I,$(INCLUDES)) $(addprefix -W,$(WARNINGS)) $(CFLAGS) -c -o $(OBJ)/$(ARCH)/$(@F) $<

# Serial monitor
monitor: stop
	@$(TERM) "$(SERIAL_MONITOR) $(SERIAL_TTY) $(SERIAL_BAUD_RATE)" &

stop:
	@for PID in `pgrep "$(SERIAL_MONITOR)"`; do kill "$$PID"; done
