import pyla
import sys

# Sequence sources / filters / sinks

def print_ascii(seq, log = sys.stderr):
    """Print byte stream as ascii."""
    for b in seq:
        log.write(chr(b))
        log.flush()

def print_hex(seq, count_init = 0, log = sys.stderr):
    """Print byte stream as hex."""
    count = count_init
    for b in seq:
        if 0 == (count % 16):
            log.write("\n%08X " % count)
        log.write("%02X " % b)
        log.flush()
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


def dump_uart(channel=0, log=sys.stderr, dump=print_hex):
    uart = pyla.uart()
    gen = saleae_analyzer(uart)
    uart.set_channel(channel)
    dump(gen, log=log)

def apply_config(obj, config):
    print("config: %s" % obj)
    if config:
        for (key, val) in config.items():
            try:
                print("config: %s=%d" % (key, val))
                getattr(obj, 'set_' + key)(val)
            except Exception as e:
                print("WARNING: set_%s not defined: %s" % (key, e))
    return obj

def dump_syncser(config={}, log=sys.stderr, dump=print_hex):
    syncser = pyla.syncser()
    apply_config(syncser, config)
    gen = saleae_analyzer(syncser)
    dump(gen, log=log)

def dump_diff(log = sys.stderr):
    print_hex(saleae_analyzer(pyla.diff()),log=log)
        
