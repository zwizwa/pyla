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

    def test_byte(inbytes):
        idle = [1,1,1]
        bits = idle
        for inbyte in inbytes:
            bits = bits + uart_frame_nopar(inbyte)
        bits = bits + idle
        # print(bits)
        ov_bytes = bytes(oversample(bits, ov))
        output = uart.process(ov_bytes)
        check(list(output), inbytes, "uart in-out")
        
    # FAILS?
    for i in range(256):
        test_byte([i])

    for i in range(256):
        test_byte([i,i,i])

    

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

        

def test_stack():
    p = pyla.stack_program()
    s = pyla.chunk_stack()
    snk = pyla.stack_op_sink(p,s)
    b = [1,2,3]
    snk.write(b)
    check(list(s.top_copy()), b, "write to pop")

    s.clear()
    p.compile_dup()
    p.compile(pyla.diff())
    snk.write([1,1,0,0,1,1])
    check([1,0,1], list(s.top_copy()), "diff")




test_stack()
test_uart()
test_pyla_memmap()
# test_saleae()


print("IGNORE error below:")

