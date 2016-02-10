OBJ=obj
OUT=out
FNAME=a
CFLAGS=-Os -g0 -Wall
INCLUDES=\
	src/timer/api \
	src/timer/cfg \
	src/os/api \
	src/os/cfg \

all: clean prepare $(TARGET)

avr: CFLAGS += -DTARGET=ARDUINO -mmcu=atmega328p
avr: CC = avr-gcc
avr: timer os elf
	avr-objcopy -O ihex $(OUT)/$(TARGET)/$(FNAME).elf $(OUT)/$(TARGET)/$(FNAME).hex
	
linux: CFLAGS += -DTARGET=LINUX_x86
linux: CC = gcc
linux: timer os elf

clean:
	@echo "Cleaning workspace"
	@rm -Rf $(OBJ)

prepare:
	@echo "Preparing workspace"
	@mkdir -p $(OBJ) $(OUT)/$(TARGET)

# Each software component must be created here
timer: src/timer/src/timer.o
os: src/os/src/os.o src/os/cfg/os_cfg.o

# Generic rules for compiling objects
%.o: %.c
	$(CC) $(addprefix -I,$(INCLUDES)) $(CFLAGS) -c -o $(OBJ)/$(@F) $<

# Link all objects into the final ELF
elf:
	$(CC) $(OBJ)/*.o -o $(OUT)/$(TARGET)/$(FNAME).elf