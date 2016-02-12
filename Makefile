CC=avr-gcc
OBJCOPY=avr-objcopy
OBJ=obj
OUT=out
FNAME=a
CFLAGS=-Os -g0 -Wall -mmcu=$(ARCH)
ARCH=atmega328p
SERIAL_TTY = /dev/ttyACM0
INCLUDES=\
	src/timer/api \
	src/timer/cfg \
	src/os/api \
	src/os/cfg \
	src/app/api \

all: clean prepare hex

elf: timer os app
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
app: src/app/src/app.o
timer: src/timer/src/timer.o
os: src/os/src/os.o src/os/cfg/os_cfg.o

# Generic rules for compiling objects
%.o: %.c
	$(CC) $(addprefix -I,$(INCLUDES)) $(CFLAGS) -c -o $(OBJ)/$(@F) $<

# Link all objects into the final ELF