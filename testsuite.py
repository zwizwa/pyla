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

    print("test_uart done")


                               
# SALEAE
def test_saleae():
    devices = pyla.devices()
    d = devices[0]
    b = pyla.blackhole()
    d.connect_sink(b)
    time.sleep(1)
    print("test_saleae done")



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
    print("test_buf done")

#def test_pyla_memmap():
#    print("test pila.memmap")
#    buf = pylacore.memmap("/tmp/pyla.memmap.bin", 1200000)
#    test_buf(buf)

def test_memmap():
    print("test pila.file")
    buf = pyla.memmap("/tmp/pyla.file.bin", 12000000)
    test_buf(buf)
    print("test_memmap done")

        

def test_stack():
    # Create stack program
    p = pyla.stack_program()
    p.dup() # duplicate input
    p.op(pyla.diff()) # perform diff operation


    p = pyla.program().dup().op(pyla.diff())

    # Wrap it into a sink API
    snk = pyla.stack_op_sink(p._p)

    # Perform a dummy run to determine the number of outputs.
    snk.write([])

    # Attach output sinks
    b = []
    for i in range(snk.nb_outputs()):
        m = pyla.memory()
        b.append(m)
        snk.connect_output(i, m)

    # Push data into sink.
    indata = [1,1,0,0,1,1]
    snk.write(indata)

    # The rults appear in the buffers.
    # First is result of diff operation
    check([1,0,1], list(b[0].read()), "b1")
    # Second is the duplicated input
    check(indata,  list(b[1].read()), "b2")

    print("test_stack done")



test_uart()
test_stack()
test_memmap()
# test_saleae()


print("IGNORE error below:")

    
