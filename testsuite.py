import pyla
import pylacore
from tools import *
import time
import sys
from stream import *

def check(cond, msg):
    if not cond:
        raise NameError(msg)
    print("PASS: %s" % msg)




# It's nice to be able to connect up object hierarchies in
# Python, i.e. to not excessively wrap the base classes with
# smart_ptr.  However, this makes it hard to


# UART
def test_uart():
    uart = pyla.uart()
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

    # output = uart.process_f(ov_bytes)
    output = pyla.process(uart, ov_bytes)


    # print(bytes(output))
    check(output[0] == inbyte, "uart byte %02X" % inbyte)


                               
# SALEAE
def test_saleae():
    devices = pyla.devices()
    d = devices[0]
    b = pyla.blackhole()
    d.connect_sink(b)
    time.sleep(1)



def test_file():
    b = bytes(list(range(256)))
    b = b + b
    b = b + b
    b = b + b
    b = b + b
    b = b + b
    n = 120000000
    print("create")
    buf = pylacore.file("/tmp/pyla.bin", n)
    print("clear")
    buf.clear()
    print("write")
    for x in range(1 + int(n/len(b))):
        pylacore.write(buf, b)

    print("read")
    for x in range(1 + int(n/len(b))):
        b_ = pylacore.read(buf, len(b))
        # print(len(b_))
    print("done")

    
        
    





test_uart()
# test_saleae()
test_file()
