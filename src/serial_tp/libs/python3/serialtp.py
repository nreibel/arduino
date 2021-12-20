#!/usr/bin/env python3

import struct

from serial import Serial, SerialException

RETCODE_OK       = 0x00
FRAME_HEADER     = 0xA0
FRAME_TERMINATOR = b'\x0D'

class SerialTPException(Exception):

    # Generic SerialTP return values
    ERROR         = 0x01
    OUT_OF_BOUNDS = 0x02

    # SerialTP Protol errors
    PROTOCOL         = 0x10
    INVALID_HEADER   = 0x11
    INVALID_DATA_LEN = 0x12

    # SerialTP User Callback errors
    FUNCTION_INVALID = 0x20
    ADDRESS_INVALID  = 0x21
    DATA_INVALID     = 0x22

    def __init__(self, code:int=ERROR):
        self.code = code


class SerialTP:

    def __init__(self, tty: str, baud: int):
        self._ser = Serial(tty, baud)
        for i in range(5):
            data = self._ser.read(5)
            if data.decode('utf8') == 'READY': return;

        raise Exception('Wrong handshake received')


    def __del__(self):
        try: self._ser.close()
        except AttributeError: pass # Object was not instanciated


    def send_command(self, fn, addr = 0, data = None):
        # Send request header
        length = 0 if data == None else len(data)
        cmd = struct.pack('BBBB', FRAME_HEADER, fn, addr, length)
        self._ser.write(cmd)

        # Send request data
        if data != None:
            self._ser.write(data)

        # Send frame terminator
        self._ser.write(FRAME_TERMINATOR)

        # Processing on client side...

        # Read response header
        rsp = self._ser.read(2)
        (status, length) = struct.unpack('BB', rsp)

        # Read response data
        data = None
        if length > 0:
            data = self._ser.read(length)

        # Read frame terminator
        term = self._ser.read(1)

        if term != FRAME_TERMINATOR: raise SerialTPException(SerialTPException.PROTOCOL)
        elif status == RETCODE_OK: return True, data
        else: return False, status


    def read(self, fn, addr):
        try:
            (status, retval) = self.send_command(fn, addr)
            if not status: raise SerialTPException(retval)
            else: return retval
        except struct.error:
            raise SerialTPException(SerialTPException.PROTOCOL)


    def write(self, fn, addr, data):
        try:
            (status, retval) = self.send_command(fn, addr, data)
            if not status: raise SerialTPException(retval)
        except struct.error:
            raise SerialTPException(SerialTPException.PROTOCOL)

    def read_format(self, fn, addr, fmt):
        data = self.read(fn, addr)
        try: return struct.unpack(fmt, data)
        except struct.error: raise SerialTPException(SerialTPException.INVALID_DATA_LEN)


    def write_string(self, fn, addr, string):
        data = string.encode('utf8')
        self.write(fn, addr, data)


    def write_byte(self, fn, addr, byte):
        try: data = struct.pack('B', byte)
        except struct.error: raise SerialTPException(SerialTPException.INVALID_DATA_LEN)
        else: self.write(fn, addr, data)


    def write_word(self, fn, addr, byte):
        try: data = struct.pack('H', byte)
        except struct.error: raise SerialTPException(SerialTPException.INVALID_DATA_LEN)
        else: self.write(fn, addr, data)


    def read_string(self, fn, addr):
        data = self.read(fn, addr)
        return data.decode('utf8')


    def read_byte(self, fn, addr):
        try: return self.read_format(fn, addr, 'B')[0];
        except IndexError: raise SerialTPException(SerialTPException.DATA_INVALID)


    def read_word(self, fn, addr):
        try: return self.read_format(fn, addr, 'H')[0];
        except IndexError: raise SerialTPException(SerialTPException.DATA_INVALID)


    def read_float(self, fn, addr):
        try: return self.read_format(fn, addr, 'f')[0];
        except IndexError: raise SerialTPException(SerialTPException.DATA_INVALID)
