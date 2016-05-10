CC=avr-gcc
OBJCOPY=avr-objcopy
OBJ=obj
OUT=out
FNAME=a
CFLAGS=-Os -g0 -Wall -mmcu=$(ARCH)
ARCH=atmega328p
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
	src/uss/api \
	src/uss/cfg \
	src/stack/api \
	src/pwm/api \
	src/pwm/cfg \
	src/math/api \
	src/app/api \
	src/app/cfg \
	src/lcd/api \
	src/lcd/cfg \

all: clean prepare hex

elf: timer os serial port app
	$(CC) $(CFLAGS) $(OBJ)/*.o -o $(OUT)/$(FNAME).elf
	
hex: elf
	$(OBJCOPY) -O ihex $(OUT)/$(FNAME).elf $(OUT)/$(FNAME).hex

clean:
	@echo "Cleaning workspace"
	@rm -Rf $(OBJ)

prepare:
	@echo "Preparing workspace"
	@mkdir -p $(OBJ) $(OUT)

upload:
	avrdude -c arduino -p $(ARCH) -P $(SERIAL_TTY) -U flash:w:$(OUT)/$(FNAME).hex

# Each software component must be created here
app: src/app/src/app.o src/app/cfg/app_cfg.o
timer: src/timer/src/timer.o
os: src/os/src/os.o src/os/cfg/os_cfg.o
serial: src/serial/src/serial.o
port: src/port/src/port.o src/port/cfg/port_cfg.o
uss: src/uss/src/uss.o src/uss/cfg/uss_cfg.o
pwm: src/pwm/src/pwm.o src/pwm/cfg/pwm_cfg.o
stack: src/stack/src/stack.o
math: src/math/src/math.o
lcd: src/lcd/src/lcd.o


# Generic rules for compiling objects
%.o: %.c
	$(CC) $(addprefix -I,$(INCLUDES)) $(CFLAGS) -c -o $(OBJ)/$(@F) $<

# Serial monitor
monitor: stop
	xterm -e "$(SERIAL_MONITOR) $(SERIAL_TTY) $(SERIAL_BAUD_RATE)" &

stop:
	@-killall -q $(SERIAL_MONITOR) 2>/dev/null; true