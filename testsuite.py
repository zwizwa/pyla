import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")
sys.path.append(".")
sys.path.append("build")
import pylacore
from tools import *
import time

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
    output = uart.process(ov_bytes)
    # print(bytes(output))
    check(output[0] == inbyte, "uart byte %02X" % inbyte)


                               
# SALEAE
def test_saleae():

    # Wait for connection
    devices = []
    while not devices:
        time.sleep(0.1)
        devices = pylacore.saleae.devices()

    print(dir(devices[0]))

    time.sleep(1)

def test_saleae_uart():
    dev = pylacore.saleae.devices()[0]
    dev.set_buffer(pylacore.memory())

    uart = pylacore.uart()

    uart_source = pylacore.chain(uart, dev);
    while 1:
        out = uart_source.read()
        print(out)
        sleep(0.3)
        


test_uart()
test_saleae()
test_saleae_uart()

