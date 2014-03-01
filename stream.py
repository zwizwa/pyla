import pyla
import sys

# Sequence sources / filters / sinks

def print_ascii(seq):
    """Print byte stream as ascii."""
    for b in seq:
        sys.stderr.write(chr(b))

def print_hex(seq, count_init = 0):
    """Print byte stream as hex."""
    count = count_init
    for b in seq:
        if 0 == (count % 16):
            sys.stderr.write("\n%08X " % count)
        sys.stderr.write("%02X " % b)
        sys.stderr.flush()
        count += 1


def buf_gen(buf):
    """Convert pyla buffer to python byte generator."""
    while 1:
        for b in pyla.read_blocking(buf):
            yield(b)

def filter_diff(seq):
    """Pass only changing bytes."""
    last_b = 0
    for b in seq:
        if b != last_b:
            yield(b)
        last_b = b



def saleae_analyzer(op):
    """Combine saleae, analyzer, buffer to make a python sequence."""
    saleae = pyla.devices()[0]
    buf = pyla.memory()

    # use buffer as a sink, and create a new sink to pass to the
    # saleae callback (co-sink).  we'll be reading the other side of
    # buf (buffer is also a source).

    buf_op = pyla.compose_snk_op(buf, op)
    saleae.connect_sink(buf_op)
    return buf_gen(buf)

def saleae_raw():
    """Raw saleae byte sequence."""
    saleae = pyla.devices()[0]
    buf = pyla.memory()
    saleae.connect_sink(buf)
    return buf_gen(buf)




# Fully connected data dumpers.


def dump_uart(channel):
    uart = pyla.uart()
    gen = saleae_analyzer(uart)
    uart.set_channel(channel)
    print_ascii(gen)

def dump_syncser(clock=0, data=1):
    syncser = pyla.syncser()
    gen = saleae_analyzer(syncser)
    syncser.set_clock_channel(clock)
    syncser.set_data_channel(data)
    syncser.set_clock_edge(0)
    print_hex(gen)

def dump_diff():
    print_hex(filter_diff(saleae_raw()))
        
