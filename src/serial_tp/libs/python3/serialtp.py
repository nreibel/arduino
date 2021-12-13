#!/usr/bin/env python3

import struct

from serial import Serial, SerialException

RETCODE_OK       = 0x00
FRAME_TERMINATOR = b'\x0D'

class SerialTPException(Exception):

    # Generic SerialTP return values
    OK            = 0x00
    ERROR         = 0x01
    OUT_OF_BOUNDS = 0x02

    # SerialTP Protol errors
    ERROR_INVALID_HEADER   = 0x10
    ERROR_INVALID_DATA_LEN = 0x11

    # SerialTP User Callback errors
    ERROR_FUNCTION_INVALID = 0x20
    ERROR_ADDRESS_INVALID  = 0x21
    ERROR_DATA_INVALID     = 0x22

    def __init__(self, code):
        self.code = code


class SerialTP:


    def __init__(self, tty: str, baud: int):
        self._ser = Serial(tty, baud)
        data = self._ser.read(5)

        if data.decode('utf8') != 'READY':
            raise Exception('Wrong handshake received')


    def __del__(self):
        try: self._ser.close()
        except AttributeError: pass # Object was not instanciated


    def send_command(self, fn, addr = 0, data = None):
        # Send request header
        length = 0 if data == None else len(data)
        cmd = struct.pack('BBBB', 0xA0, fn, addr, length)
        self._ser.write(cmd)

        # Send request data
        if data != None:
            self._ser.write(data)

        # Send frame terminator
        self._ser.write(FRAME_TERMINATOR)

        # Processing on client side...

        # Read response header
        # TODO : read until timeout
        rsp = self._ser.read(2)
        (status, data_len) = struct.unpack('BB', rsp)

        # Read response data
        data = None
        if data_len > 0:
            data = self._ser.read(data_len)

        # Read frame terminator
        term = self._ser.read(1)

        if term != FRAME_TERMINATOR:
            print('Expected EOF')
            return False, None

        elif status == RETCODE_OK:
            return True, data

        else:
            return False, status


    def write_string(self, fn, addr, string):
        data = string.encode('utf8')
        (status, retval) = self.send_command(fn, addr, data)
        if not status: raise SerialTPException(retval)


    def write_byte(self, fn, addr, byte):
        try: data = struct.pack('B', byte)
        except struct.error: raise Exception('Format error')
        (status, retval) = self.send_command(fn, addr, data)
        if not status: raise SerialTPException(retval)


    def write_word(self, fn, addr, byte):
        try: data = struct.pack('H', byte)
        except struct.error: raise Exception('Format error')
        (status, retval) = self.send_command(fn, addr, data)
        if not status: raise SerialTPException(retval)


    def read_string(self, fn, addr):
        (status, retval) = self.send_command(fn, addr)
        if status: return retval.decode('utf8')
        else: raise SerialTPException(retval)


    def read_byte(self, fn, addr):
        (status, retval) = self.send_command(fn, addr)
        if status: return struct.unpack('B', retval)[0]
        else: raise SerialTPException(retval)


    def read_word(self, fn, addr):
        (status, retval) = self.send_command(fn, addr)
        if status: return struct.unpack('H', retval)[0]
        else: raise SerialTPException(retval)


    def read_float(self, fn, addr):
        (status, retval) = self.send_command(fn, addr)
        if status: return struct.unpack('f', retval)[0]
        else: raise SerialTPException(retval)
