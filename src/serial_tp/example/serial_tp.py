#!/usr/bin/env python3

import sys
import time
import serial
import struct

SERIAL_TTY = '/dev/ttyUSB0'
SERIAL_BAUDRATE = 19200

SERIAL_TP_FRAME_TERMINATOR = b'\x0D'
SERIAL_TP_REQUEST_HEADER = b'\xA0'

RETCODE_OK = 0x00

FUNCTION_WRITE_DATA   = 0x10
FUNCTION_READ_DATA    = 0x11

DATA_X  = 0x01
DATA_Y  = 0x02


# # # # # # # # # # #
# HELPER FUNCTI0ON  #
# # # # # # # # # # #


def send_command(ser, fn, addr = 0, data = None):

    # Send request header
    cmd = struct.pack('BBB', 0xA0, fn, addr)
    ser.write(cmd)

    # Send request data
    if data is None:
        ser.write(b'\x00')
    else:
        data_len = struct.pack('B', len(data))
        ser.write(data_len)
        ser.write(data)

    # Send frame terminator
    ser.write(SERIAL_TP_FRAME_TERMINATOR)

    # Processing on client side...

    # Read response header
    # TODO : read until timeout
    rsp = ser.read(2)
    (status, data_len) = struct.unpack('BB', rsp)

    # Read response data
    data = None;
    if data_len > 0:
        data = ser.read(data_len)

    # Read frame terminator
    term = ser.read(1)

    if term != SERIAL_TP_FRAME_TERMINATOR:
        return (False, None)

    elif status == RETCODE_OK:
        return (True, data)

    else:
        return (False, status)

# # # # # # # # # # # # # #
# APPLICATION STARTS HERE #
# # # # # # # # # # # # # #


ser = serial.Serial(SERIAL_TTY, SERIAL_BAUDRATE)  # open serial port
time.sleep(1)
data = ser.read(5);

if data.decode('utf8') != 'READY':
    print('Wrong handshake received')
    sys.exit(1)

print("opened " + ser.name)

print('read startup data')
(status, retval) = send_command(ser, FUNCTION_READ_DATA, DATA_X)
if status: print('  x is ' + str(retval))
else: print('  error ' + str(retval))

print('write data = 0x2')
(status, retval) = send_command(ser, FUNCTION_WRITE_DATA, DATA_X, b'\x02')
if status: print('  success');
else: print('  error ' + str(retval));

print('read new value')
(status, retval) = send_command(ser, FUNCTION_READ_DATA, DATA_X)
if status: print('  x is now ' + str(retval))
else: print('  error ' + str(retval))

print('write to wrong function id')
(status, retval) = send_command(ser, 0xFF, 0x00, b'\x00')
if status: print('  success');
else: print('  error ' + str(retval));

print('write to wrong data id')
(status, retval) = send_command(ser, FUNCTION_WRITE_DATA, 0xFF, b'\x00')
if status: print('  success');
else: print('  error ' + str(retval));

print('write wrong data length')
(status, retval) = send_command(ser, FUNCTION_WRITE_DATA, 0xFF, b'\x00\x00')
if status: print('  success');
else: print('  error ' + str(retval));

ser.close()
print('ok');
