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

def filter_diff(seq):
    """Pass only changing bytes."""
    last_b = 0
    for b in seq:
        if b != last_b:
            yield(b)
        last_b = b

def saleae_with(op, record=None, buftype=['memory']):
    """Combine saleae, analyzer+config, buffer to make a python sequence."""
    saleae = pyla.devices()[0]
    buf = getattr(pyla, buftype[0])(*buftype[1:])
    if record:
        buf.set_log(record)

    # use buffer as a sink, and create a new sink to pass to the
    # saleae callback (co-sink).  we'll be reading the other side of
    # buf (buffer is also a source).

    buf_op = pyla.compose_snk_op(buf, op)

    # FIXME: Add possibility for difference encoding frontend?
    # buf_op = pyla.compose_snk_op(buf_op, pyla.diff())
    
    saleae.connect_sink(buf_op)
    return buf.bytes()

def saleae_raw():
    """Raw saleae byte sequence."""
    saleae = pyla.devices()[0]
    buf = pyla.memory()
    saleae.connect_sink(buf)
    return buf.bytes()





