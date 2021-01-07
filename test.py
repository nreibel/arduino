#!/usr/bin/env python3

import time
import serial

ser = serial.Serial('/dev/ttyUSB0', 19200)  # open serial port
time.sleep(3)

print("opened " + ser.name)         # check which port was really used

# Clear screen
ser.write( b'\x00\x20\x00\x00\x0d' )
ret = ser.read(2)
print("got " + str(ret))

# Write data CURSOR_X
ser.write( b'\x00\x10\x01\x01\x02\x0d' )
ret = ser.read(2)
print("got " + str(ret))

# Write data CURSOR_Y
ser.write( b'\x00\x10\x02\x01\x02\x0d' )
ret = ser.read(2)
print("got " + str(ret))

# Print "Hello"
ser.write( b'\x00\x21\x00\x05\x48\x65\x6c\x6c\x6f\x0d' )
ret = ser.read(2)
print("got " + str(ret))

# Read back cursor position
ser.write( b'\x00\x11\x01\x00\x0d' )
ret = ser.read(2)
len = ret[1]
cx = ser.read(len)
print("Cursor X position is " + str(cx) )

ser.write( b'\x00\x11\x02\x00\x0d' )
ret = ser.read(2)
len = ret[1]
cy = ser.read(len)
print("Cursor Y position is " + str(cy))

# Read back undefined data
ser.write( b'\x00\x11\x03\x00\x0d' )
ret = ser.read(2)
print("got " + str(ret))

ser.close()
print('ok');
