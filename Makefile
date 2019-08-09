# Path to build tools
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
UPLOAD=avrdude

# Output files
OBJ=obj
OUT=bin
FNAME=out_$(ARCH)

# Compilation options and flags
ARCH=atmega328p
WARNINGS=all extra undef
CFLAGS=-O3 -g0 -mmcu=$(ARCH) -ffunction-sections -fdata-sections
LDFLAGS=-Wl,-gc-sections -Wl,--relax

# Serial config
SERIAL_TTY=/dev/ttyACM0
SERIAL_MONITOR=screen
SERIAL_BAUD_RATE=19200

# avrisp  : use arduino as ISP to flash another chip
# arduino : usual Arduino flash process
PROGRAMMER=arduino

INCLUDES=\
	src/timer/api \
	src/timer/cfg \
	src/os/api \
	src/os/cfg \
	src/serial/api \
	src/serial/cfg \
	src/port/api \
	src/port/cfg \
	src/app/api \
	src/app/cfg \
	src/eeprom/api \
	src/eeprom/cfg \
	src/keys/api \
	src/keys/cfg \
	src/uss/api \
	src/uss/cfg \
	src/stack/api \
	src/pwm/api \
	src/pwm/cfg \
	src/math/api \
	src/lcd/api \
	src/lcd/cfg \
	src/spi/api \
	src/spi/cfg \
	src/st7735/api \
	src/st7735/cfg


all: clean prepare hex

hex: timer os serial port eeprom keys app uss pwm stack math lcd spi st7735
	@echo "Linking object files..."
	@$(CC) $(LDFLAGS) $(CFLAGS) $(OBJ)/$(ARCH)/*.o -o $(OUT)/$(FNAME).elf
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

upload:
	$(UPLOAD) -b 115200 -c $(PROGRAMMER) -p $(ARCH) -P $(SERIAL_TTY) -U flash:w:$(OUT)/$(FNAME).hex

# Each software component must be created here
app:    src/app/src/app.o src/app/cfg/app_cfg.o
timer:  src/timer/src/timer.o
os:     src/os/src/os.o src/os/src/utils.c src/os/cfg/os_cfg.o
serial: src/serial/src/serial.o
port:   src/port/src/port.o src/port/cfg/port_cfg.o
eeprom: src/eeprom/src/eeprom.o
keys:   src/keys/src/keys.o src/keys/cfg/keys_cfg.o
uss:    src/uss/src/uss.o src/uss/cfg/uss_cfg.o
pwm:    src/pwm/src/pwm.o src/pwm/cfg/pwm_cfg.o
stack:  src/stack/src/stack.o
math:   src/math/src/math.o
lcd:    src/lcd/src/lcd.o
spi:    src/spi/src/spi.o src/spi/cfg/spi_cfg.o
st7735: src/st7735/src/st7735.o src/st7735/cfg/st7735_cfg.o

# Generic rules for compiling objects
%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(addprefix -I,$(INCLUDES)) $(addprefix -W,$(WARNINGS)) $(CFLAGS) -c -o $(OBJ)/$(ARCH)/$(@F) $<

# Serial monitor
monitor: stop
	@xterm -e "$(SERIAL_MONITOR) $(SERIAL_TTY) $(SERIAL_BAUD_RATE)" &

stop:
	@for PID in `pgrep "$(SERIAL_MONITOR)"`; do kill "$$PID"; done
