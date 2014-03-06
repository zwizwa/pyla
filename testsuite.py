import pyla
import pylacore
from tools import *
import time
import sys
from stream import *



def check(val, exp, msg):
    if val != exp:
        raise NameError(msg + ": %s != %s" % (val,exp))


# UART
def test_uart():
    uart = pyla.uart()
    br = 9600
    ov = 16
    sr = br * ov
    uart.set_baudrate(br)
    uart.set_samplerate(sr)
    uart.set_channel(0)

    def test_byte(inbyte):
        idle = [1,1,1]
        bits = idle + uart_frame_nopar(inbyte) + idle
        ov_bytes = bytes(oversample(bits, ov))
        output = uart.process(ov_bytes)
        check(list(output), [inbyte], "uart in-out")
        
    # FAILS?
    for i in range(256):
        test_byte(i)
    # test_byte(0x55)
    # test_byte(0x0F)

    # print(bytes(output))



                               
# SALEAE
def test_saleae():
    devices = pyla.devices()
    d = devices[0]
    b = pyla.blackhole()
    d.connect_sink(b)
    time.sleep(1)



def test_buf(buf):
    b = bytes(list(range(256)))
    n = 1200000
    print("write")
    for x in range(1 + int(n/len(b))):
        buf.write(b)

    print("read")
    b_ = [1]
    while len(b_) > 0:
        b_ = buf.read()
        print(len(b_))
    print("done")

#def test_pyla_memmap():
#    print("test pila.memmap")
#    buf = pylacore.memmap("/tmp/pyla.memmap.bin", 1200000)
#    test_buf(buf)

def test_pyla_memmap():
    print("test pila.file")
    buf = pyla.memmap("/tmp/pyla.file.bin", 12000000)
    test_buf(buf)

        





test_uart()
test_pyla_memmap()
# test_saleae()


print("IGNORE error below:")

