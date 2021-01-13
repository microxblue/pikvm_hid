import sys
import serial
import time
from   ctypes import *

class MS_HID_PKT (Structure):
    _pack_ = 1
    _fields_ = [
        ('btn',  c_uint8),
        ('x',    c_uint16),
        ('y',    c_uint16),
        ('s',    c_uint16),
        ]

kb_sym_key_map = {
    '!': 0x11E,
    '@': 0x11F,
    '#': 0x120,
    '$': 0x121,
    '%': 0x122,
    '^': 0x123,
    '&': 0x124,
    '*': 0x125,
    '(': 0x126,
    ')': 0x127,
    '0': 0x27,
    '\n': 0x28,
    '\r': 0x28,
    '\b': 0x2A,
    '\t': 0x2B,
    ' ': 0x2C,
    '_': 0x12D,
    '-': 0x2D,
    '+': 0x12E,
    '=': 0x2E,
    '{': 0x12F,
    '[': 0x2F,
    '}': 0x130,
    ']': 0x30,
    '|': 0x131,
    '\\': 0x31,
    ':': 0x133,
    ';': 0x33,
    '"': 0x134,
    '\'': 0x34,
    '~': 0x135,
    '`': 0x35,
    '<': 0x136,
    ',': 0x36,
    '>': 0x137,
    '.': 0x37,
    '?': 0x138,
    '/': 0x38,
}

def ascii_to_hid(ch):
    shift = 0
    c = ch.encode()[0]
    if (c >= ord('A')) and (c <= ord('Z')):
        c = ord('a') + (c - ord('A'))
        shift = 0x100

    if (c >= ord('a')) and (c <= ord('z')):
        c = c - ord('a')
        return (c + 4) | shift

    if (c >= ord('1')) and (c <= ord('9')):
        c = c - ord('0')
        return c + 0x1d
    return kb_sym_key_map[chr(c)]

def str_to_kb_hid(keystr):
    grp_list = []

    keys  = bytearray()
    shift = 0
    sep   = False
    for ch in keystr + '\x00':
        shift_next = shift
        if ch == '\x00':
            sep = True
        else:
            key = ascii_to_hid(ch)
            if (key & 0x100) != shift:
                shift_next = key & 0x100
                sep   = True
            if len(keys) >= 6:
                sep   = True
        if sep:
            grp_list.append((b'\x02' if shift else b'\x00') + b'\x00' + bytearray(keys))
            keys = bytearray()
            sep  = False
        keys.append (key & 0xff)
        shift = shift_next
    return grp_list


def swap16(x):
    return ((x & 0xff) << 8) + ((x >> 8) & 0xff)


class KVM_HID:
    def __init__(self, comport):
        self.ser = serial.Serial(comport, 115200, timeout=0)

    def wait_rsp(self):
        data_str = ''
        loop = 0
        while loop < 100:
            if (self.ser.inWaiting() > 0):
                data_str += self.ser.read(self.ser.inWaiting()).decode('ascii')
                if '\r' in data_str or '\n' in data_str:
                    break
            time.sleep(0.01)
            loop += 1
        return data_str


    def send_hid_kb_pkt (self, keystr):
        self.ser.write(b'@%s\r' % keystr)
        rsp1 = self.wait_rsp()
        self.ser.write(b'@\x00\x00\r')
        rsp2 = self.wait_rsp()
        if '$OK' in rsp1 and '$OK' in rsp2:
            return 0
        else:
            return 1

    def send_hid_ms_pkt (self, data):
        self.ser.write(b'#%s\r' % bytearray(data))
        rsp = self.wait_rsp()
        if '$OK' in rsp:
            return 0
        else:
            return 1

def main ():
    if len(sys.argv) < 2:
        print ('Usage:\n  python %s serial_device\n' % sys.argv[0])
        return 1

    cont    = True
    comport = sys.argv[1]
    hid = KVM_HID (comport)

    time.sleep(0.1)

    # send "dir" keys to keyboard
    if 1:
        key_str  = 'dir\n'
        key_list = str_to_kb_hid(key_str)
        for keys in key_list:
            if hid.send_hid_kb_pkt (keys):
                raise Exception ('Failed to send keyboard HID packet !')

    # move mouse
    ms_hid = MS_HID_PKT()
    ms_hid.btn = 0
    ms_hid.x   = 1
    ms_hid.y   = 1
    ms_hid.s   = 0
    for i in range(4):
        ms_hid.x  = 1000 + (i  & 1) * 4000
        ms_hid.y  = 1000 + (i >> 1) * 4000
        if hid.send_hid_ms_pkt (ms_hid):
            raise Exception ('Failed to send mouse HID packet !')
        time.sleep (.5)

if __name__ == '__main__':
    sys.exit(main())
