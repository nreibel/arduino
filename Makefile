CC = gcc
TARGET = arduino-demo
OBJ_PATH = ./obj
OBJECTS = src/timer/src/timer.o src/os/src/os.o src/os/cfg/os_cfg.o
INCLUDES = -Isrc/timer/api -Isrc/timer/cfg -Isrc/os/api -Isrc/os/cfg

# Generic targets
all: clean prepare $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJ_PATH)/*.o -o $@

clean:
	@-rm -rf $(OBJ_PATH) $(TARGET)
	
prepare:
	@mkdir -p $(OBJ_PATH)
	
# Compilation
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $(OBJ_PATH)/$(@F) $<

# Serial monitor
monitor: stop
	@xterm -e "$(SERIAL_MONITOR) $(SERIAL_TTY) $(SERIAL_BAUD_RATE)" &

stop:
	@-killall -q $(SERIAL_MONITOR) 2>/dev/null; true

# Upload to Arduino
upload: stop
	avrdude -c arduino -p $(ARCH) -P $(SERIAL_TTY) -U flash:w:$(TARGET)