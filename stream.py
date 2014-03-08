import pyla
import sys

# In Python it seems most convenient to work with sequences instead of
# sample chunks and non-blocking reads.  This file contains some
# utilities on top of pyla.buffer_wrapper.bytes()

# SINKS
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

# FILTERS
def filter_diff(seq):
    """Pass only changing bytes."""
    last_b = 0
    for b in seq:
        if b != last_b:
            yield(b)
        last_b = b

# SOURCES
def saleae_with(op, record=None, buftype=['memory']):
    """Combine saleae, analyzer+config, buffer to make a python sequence."""
    saleae = pyla.devices()[0]
    op.set_samplerate(saleae.get_samplerate())
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





# MULTIPLE OUTPUTS (not abstracted as streams)
# FIXME: not sure yet how to present this in API...

class multibuf:
    def __init__(self, stack_prog):
        self._p = stack_prog
        self._snk = pyla.stack_op_sink(self._p)
        self._buf = []
        for i in range(self._snk.nb_outputs()):
            buf = pyla.memory()
            self._buf.append(buf)
            self._snk.connect_output(i, buf)
    def write(self, b):
        self._snk.write(b)
    def read_multi(self):
        return [b.read() for b in self._buf]

    def connect(self,dev):
        dev.connect_sink(self._snk)
        return self
