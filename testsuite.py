import pyla
import pylacore
from tools import *
import time
import sys
from stream import *
import random
from props import *

def check(val, exp, msg):
    if val != exp:
        raise NameError(msg + ": %s != %s" % (val,exp))



class uart_test:
    def __init__(self, br, ov):
        self.br = br
        self.ov = ov
        self.uart = pyla.uart()
        self.uart.set_baudrate(self.br)
        self.uart.set_samplerate(self.sr())
        self.uart.set_channel(0)

    def sr(self):
        return self.br * self.ov
    
    def prop_gen_parse(self, inbytes, n_idle=3):
        idle = [1 for i in range(n_idle)]
        bits = idle
        for inbyte in inbytes:
            bits = bits + uart_frame_nopar(inbyte)
        bits = bits + idle
        # print(bits)
        ov_bytes = bytes(oversample(bits, self.ov))
        output = self.uart.process(ov_bytes)
        check(list(output), inbytes, "uart in-out")
    

def test_uart():

    def prop_br_ov(br, ov):
        ut = uart_test(br, ov)

        # Manual
        for i in range(256):
            ut.prop_gen_parse([i])

        # Generated tests.
        forall(100, ut.prop_gen_parse, a_list(a_byte), a_range(0,13))

    forall(10, prop_br_ov, a_range(9600,115200), a_range(1,16))

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
    p.dup()            # duplicate input
    p.op(pyla.dedup()) # perform deduplication operation

    # Wrap it as a sink.
    snk = pyla.stack_op_sink(p)

    # Attach output sinks
    b = []
    for i in range(snk.nb_outputs()):
        m = pyla.memory()
        b.append(m)
        snk.connect_output(i, m)

    # Push data into sink.
    indata = [1,1,0,0,1,1]
    snk.write(indata)

    # The results appear in the buffers.
    # First is result of diff operation
    check([1,0,1], list(b[0].read()), "b[0]")
    # Second is the duplicated input
    check(indata,  list(b[1].read()), "b[1]")

    print("test_stack done")

def test_multibuf():
    # Create stack program
    p = pyla.stack_program()
    p.dup()
    p.save()
    p.op(pyla.dedup())
    p.save()

    # Wrap it as a sink.
    mb = multibuf(p)

    # Push data into sink.
    indata = [1,1,0,0,1,1]
    mb.write(indata)

    # Read results
    b = mb.read_multi()
    check([1,0,1], list(b[1]), "b[1]")
    check(indata,  list(b[0]), "b[0]")
    
    print("test_multibuf done")







test_uart()
test_stack()
test_multibuf()
test_memmap()
# test_saleae()



print("IGNORE error below:")

    
