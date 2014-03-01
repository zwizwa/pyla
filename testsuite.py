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


# FIXME: Some memory management is needed for objects accessible both
# in python and the "connection" objects in C++.  For now each object
# is simply registered forever (leaked) using these wrapper functions.

class pyla_registry:
    def __init__(self):
        self.objects = []
    def __getattr__(self, attr):
        cons = getattr(pylacore, attr)
        def proxy(*args):
            ob = cons(*args)
            self.objects.append(ob)
            return ob
        return proxy

pyla = pyla_registry()


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
    output = pylacore.process(uart, ov_bytes)


    # print(bytes(output))
    check(output[0] == inbyte, "uart byte %02X" % inbyte)


def saleae_wait_connection():
    # Wait for connection
    devices = []
    while not devices:
        time.sleep(0.1)
        devices = pylacore.saleae.devices()
    return devices

                               
# SALEAE
def test_saleae():
    devices = saleae_wait_connection()
    d = devices[0]
    b = pyla.blackhole()
    d.connect_sink(b)
    time.sleep(1)

def test_saleae_uart():
    devices = saleae_wait_connection()
    saleae = devices[0]

    buf = pyla.memory()  # we'll be reading this one
    uart = pyla.uart()

    # use buffer as a sink, and create a new sink to pass to the
    # saleae callback (co-sink)
    buf_uart = pyla.compose_snk_op(buf, uart)

    saleae.connect_sink(buf_uart)

    while 1:
        out = bytes(pylacore.read(buf))
        if len(out):
            sys.stderr.write(out.decode('ascii','ignore'))
        else:
            time.sleep(0.1)

        


test_uart()
test_saleae()
test_saleae_uart()

