import pyla
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





# test_uart()
# test_saleae()
# dump_uart(0)


# DUMP SPI as ASCII
dump_syncser(
    config={'frame_channel'  : 2,
            'clock_channel'  : 0,
            'data_channel'   : 1,
            'clock_edge'     : 1,  # sampling edge: 0->1
            'clock_polarity' : 0,  # clock idle polarity
            'frame_active'   : 0},
    dump=print_hex)


# test_saleae_syncser()
# dump_diff()

# dump_syncser(frame=2,dump=print_hex)



