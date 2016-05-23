CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
UPLOAD=avrdude
ARCH=atmega328p
PROGRAMMER=arduino
OBJ=_obj
OUT=out
FNAME=out_$(ARCH)
CFLAGS=-Os -g0 -Wall -mmcu=$(ARCH)
SERIAL_TTY = /dev/ttyACM0
SERIAL_MONITOR = screen
SERIAL_BAUD_RATE = 9600
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
	src/keys/cfg
# 	src/uss/api \
# 	src/uss/cfg \
# 	src/stack/api \
# 	src/pwm/api \
# 	src/pwm/cfg \
# 	src/math/api \
# 	src/lcd/api \
# 	src/lcd/cfg \


all: prepare hex

hex: timer os serial port eeprom keys app
	@echo "Linking object files..."
	@$(CC) $(CFLAGS) $(OBJ)/$(ARCH)/*.o -o $(OUT)/$(FNAME).elf
	@echo "Creating HEX file..."
	@$(OBJCOPY) -O ihex $(OUT)/$(FNAME).elf $(OUT)/$(FNAME).hex
	@echo "=================================="
	@$(SIZE) -C --mcu=$(ARCH) $(OUT)/$(FNAME).elf
	@echo "Done"

clean:
	@echo "Cleaning workspace"
#	@find . -name "*.o" -type f -printf "Delete %p\n" -delete
	@rm -Rf $(OBJ)
	@rm -Rf $(OUT)

prepare:
	@echo "Preparing workspace"
	@mkdir -p $(OBJ)/$(ARCH) $(OUT)

upload:
	@$(UPLOAD) -c $(PROGRAMMER) -p $(ARCH) -P $(SERIAL_TTY) -U flash:w:$(OUT)/$(FNAME).hex

# Each software component must be created here
app:    src/app/src/app.o src/app/cfg/app_cfg.o
timer:  src/timer/src/timer.o
os:     src/os/src/os.o src/os/cfg/os_cfg.o
serial: src/serial/src/serial.o
port:   src/port/src/port.o src/port/cfg/port_cfg.o
eeprom: src/eeprom/src/eeprom.o
keys:   src/keys/src/keys.o src/keys/cfg/keys_cfg.o
# uss:    src/uss/src/uss.o src/uss/cfg/uss_cfg.o
# pwm:    src/pwm/src/pwm.o src/pwm/cfg/pwm_cfg.o
# stack:  src/stack/src/stack.o
# math:   src/math/src/math.o
# lcd:    src/lcd/src/lcd.o

# Generic rules for compiling objects
%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(addprefix -I,$(INCLUDES)) $(CFLAGS) -c -o $(OBJ)/$(ARCH)/$(@F) $<

# Serial monitor
monitor: stop
	@xterm -e "$(SERIAL_MONITOR) $(SERIAL_TTY) $(SERIAL_BAUD_RATE)" &

stop:
	@-killall -q $(SERIAL_MONITOR) 2>/dev/null; true
