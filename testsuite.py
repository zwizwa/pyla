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
    b = pylacore.blackhole()
    d.connect_sink(b)
    time.sleep(1)

def test_saleae_uart():
    devices = saleae_wait_connection()
    saleae = devices[0]

    buf = pylacore.memory()  # we'll be reading this one
    uart = pylacore.uart()

    # use buffer as a sink, and create a new sink to pass to the
    # saleae callback (co-sink)
    buf_uart = pylacore.compose_snk_op(buf, uart)

    saleae.connect_sink(buf_uart)

    while 1:
        out = bytes(pylacore.read(buf))
        if len(out):
            sys.stderr.write(out.decode('ascii','ignore'))
        else:
            time.sleep(0.1)

        


test_uart()
# test_saleae()
test_saleae_uart()

