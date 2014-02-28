import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")
sys.path.append(".")
sys.path.append("build")
import pylacore
from tools import *

# Core module doesn't have any functionality.  Should this be removed?
core = pylacore.pylacore()
# print(core.identify())

def check(cond, msg):
    if not cond:
        raise NameError(msg)
    print("PASS: %s" % msg)


# UART
def test_uart():
    uart = pylacore.uart()
    br = 9600
    ov = 16
    sr = br * ov
    uart.set_baudrate(br)
    uart.set_samplerate(sr)
    uart.set_channel(0)

    inbyte = 0x0F
    idle = [1,1,1]
    bits = idle + uart_frame(inbyte) + idle

    ov_bytes = bytes(oversample(bits, ov))
    # print(ov_bytes)
    output = uart.analyze(ov_bytes)
    # print(bytes(output))
    check(output[0] == inbyte, "uart byte %02X" % inbyte)

test_uart()

                               
