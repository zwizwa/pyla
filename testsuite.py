import pyla
from tools import *
import time
import sys

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


def dump_ascii(seq):
    for b in seq:
        sys.stderr.write(chr(b))

def dump_hex(seq, count_init = 0):
    count = count_init
    for b in seq:
        if 0 == (count % 16):
            sys.stderr.write("\n%08X " % count)
        sys.stderr.write("%02X " % b)
        sys.stderr.flush()
        count += 1

def buf_gen(buf):
    while 1:
        for b in pyla.read_blocking(buf):
            yield(b)

# Combine saleae, analyzer, buffer and python generator in one object.
def saleae_analyzer(op):
    saleae = pyla.devices()[0]
    buf = pyla.memory()

    # use buffer as a sink, and create a new sink to pass to the
    # saleae callback (co-sink).  we'll be reading the other side of
    # buf (buffer is also a source).

    buf_op = pyla.compose_snk_op(buf, op)
    saleae.connect_sink(buf_op)
    return buf_gen(buf)

def saleae_raw():
    saleae = pyla.devices()[0]
    buf = pyla.memory()  # we'll be reading this one
    saleae.connect_sink(buf)
    return buf_gen(buf)

def dump_uart(channel):
    uart = pyla.uart()
    gen = saleae_analyzer(uart)
    uart.set_channel(channel)
    dump_ascii(gen)

def dump_syncser(clock=0, data=1):
    syncser = pyla.syncser()
    gen = saleae_analyzer(syncser)
    syncser.set_clock_channel(clock)
    syncser.set_data_channel(data)
    syncser.set_clock_edge(0)
    dump_hex(gen)

def filter_diff(seq):
    last_b = 0
    for b in seq:
        if b != last_b:
            yield(b)
        last_b = b

def test_saleae_diff():
    dump_hex(filter_diff(saleae_raw()))
        


# test_uart()
# test_saleae()
# dump_uart(0)
dump_syncser()
# test_saleae_syncser()
# test_saleae_diff()



