#!/usr/bin/env python3

import sys
import time
import serial
import struct

SERIAL_TP_LINE_TERMINATOR = b'\x0D'
SERIAL_TP_REQUEST_HEADER = b'\xA0'

RETCODE_OK = 0x00

FUNCTION_WRITE_DATA   = 0x10
FUNCTION_READ_DATA    = 0x11
FUNCTION_CLEAR_SCREEN = 0x20
FUNCTION_PRINT_STRING = 0x21

DATA_BACKLIGHT = 0x00
DATA_CURSOR_X  = 0x01
DATA_CURSOR_Y  = 0x02
DATA_BGCOLOR   = 0x03
DATA_FGCOLOR   = 0x04

BGCOLOR_BLACK = 0x0
BGCOLOR_WHITE = 0x1

def send_command(ser, fn, addr = 0, data = None):

    cmd = struct.pack('BBB', 0xA0, fn, addr)
    ser.write(cmd)

    if data is None:
        ser.write(b'\x00')
    else:
        data_len = struct.pack('B', len(data))
        ser.write(data_len)
        ser.write(data)

    ser.write(SERIAL_TP_LINE_TERMINATOR)

    # Handle response
    rsp = ser.read(2)
    (status, datalen) = struct.unpack('BB', rsp)
    if status == RETCODE_OK:
        if datalen == 0:
            return True
        else:
            return ser.read(datalen)
    else:
        return False


# # # # # # # # # # #
# HELPER FUNCTIONS  #
# # # # # # # # # # #


def clear_screen():
    return send_command(ser, FUNCTION_CLEAR_SCREEN)

def set_backlight(val):
    data = struct.pack('B', val)
    return send_command(ser, FUNCTION_WRITE_DATA, DATA_BACKLIGHT, data)

def set_cursor_position(x, y):
    data_x = struct.pack('B', x)
    data_y = struct.pack('B', y)
    ret_x = send_command(ser, FUNCTION_WRITE_DATA, DATA_CURSOR_X, data_x)
    ret_y = send_command(ser, FUNCTION_WRITE_DATA, DATA_CURSOR_Y, data_y)
    return ret_x and ret_y

def print_text(txt):
    data = bytes(txt, 'utf8')
    ret = send_command(ser, FUNCTION_PRINT_STRING, 0, data)

def check_error(err):
    if not err:
        print('failed with ' + str(err))
        sys.exit(1)

def set_background(bg):
    data = struct.pack('B', bg)
    ret = send_command(ser, FUNCTION_WRITE_DATA, DATA_BGCOLOR, data)

def set_foreground(fg):
    data = struct.pack('B', fg)
    ret = send_command(ser, FUNCTION_WRITE_DATA, DATA_FGCOLOR, data)

def get_cursor_position():

    cx = None
    cy = None

    rsp_x = send_command(ser, FUNCTION_READ_DATA, DATA_CURSOR_X)
    rsp_y = send_command(ser, FUNCTION_READ_DATA, DATA_CURSOR_Y)

    if rsp_x:
        cx = int.from_bytes(rsp_x, 'big');

    if rsp_y:
        cy = int.from_bytes(rsp_y, 'big');

    return cx, cy


# # # # # # # # # # # # # #
# APPLICATION STARTS HERE #
# # # # # # # # # # # # # #


ser = serial.Serial('/dev/ttyUSB0', 19200)  # open serial port
time.sleep(1)
data = ser.read(5);

if data.decode('utf8') != 'READY':
    print('Wrong handshake received')
    sys.exit(1)

print("opened " + ser.name)

set_background(BGCOLOR_BLACK)
set_foreground(BGCOLOR_WHITE)
set_cursor_position(2, 10)

set_backlight(0)
clear_screen()
print_text('Hello')
set_backlight(255)

cursor = get_cursor_position()
print('Cursor position was ' + str(cursor) )

ser.close()
print('ok');
