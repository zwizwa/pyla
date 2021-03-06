import pyla
import sys

# In Python it seems most convenient to work with sequences instead of
# sample chunks and non-blocking reads.  This file contains some
# utilities on top of pyla.buffer_wrapper.bytes()

# SINKS
def print_ascii(seq, log = sys.stderr):
    """Print byte stream as ascii."""
    for b in seq:
        if b>=32 and b<127:
            log.write(chr(b))
        else:
            log.write("\\x%02X" % b)
        log.flush()

def print_console(seq, log = sys.stderr):
    """Print byte stream as raw bytes, e.g for serial console."""
    for b in seq:
        log.write(chr(b))
        log.flush()


def print_hex(seq, count_init = 0, log = sys.stderr, newline = '\n'):
    """Print byte stream as hex."""
    count = count_init
    for b in seq:
        if 0 == (count % 16):
            log.write("%s%08X " % (newline, count))
        log.write("%02X " % b)
        log.flush()
        count += 1
    return count


# One hex command sequence per line.
# Print F8 clock tick with a counter to reduce verbosity.
def print_midi(seq):
    msg = []
    nb_F8 = 0
    while True:
        b = next(seq)
        if b == 0xF8:
            nb_F8 = nb_F8 + 1
            sys.stdout.write("\r%02X (%d) " % (b,nb_F8))
        else:
            if b & 0x80:
                if nb_F8:
                    print()
                    nb_F8 = 0
                for m in msg:
                    sys.stdout.write("%02X " % m)
                msg = []
                print()
            msg.append(b)
        sys.stdout.flush()
                    


# FILTERS
def filter_diff(seq):
    """Pass only changing bytes."""
    last_b = 0
    for b in seq:
        if b != last_b:
            yield(b)
        last_b = b

# SOURCES

# This function is an example of the full architecture:
# - sampling devices push data to sinks (sampler ISR/callback calls 'write')
# - operations process input to output, but don't perform memory management
# - programs combine operations, using a chunk stack to provide shared_ptr-based chunk memory management
# - programs exposed as sinks can be attached to sampling devices.  processing results are pushed to other (low-rate) output sinks.
# - buffers adapt sink interfaces ('write' = data push) to source interfaces ('read' = data pull). these are used on the low-rate side.
# - python generators poll the low-rate buffers


def device_with(op, record=None, buftype=['memory'], device=None):
    """Combine device, analyzer+config, buffer to make a python sequence."""

    if device == None:
        device = pyla.devices()[0]

    # Pass the samplerate to the operation.
    op.set_samplerate(device.get_samplerate())

    # Create the buffer that will connect the sink and source
    # interfaces, resp. data push by analyzer and data pull by python
    # script.
    buf = getattr(pyla, buftype[0])(*buftype[1:])
    if record:
        buf.set_log(record)

    # Wrap operation in a stack program
    p = pyla.stack_program()
    p.op(op)

    # Wrap program in a "push" evaluator exposing a sink interface.
    snk = pyla.stack_op_sink(p)

    # Connect the evaluator output to the buffer.  The program has
    # only one output.
    snk.connect_output(0, buf)

    # Pass the sink interface to the sampler.
    device.connect_sink(snk)

    # The other side of the buffer then presents a source interface
    # which is polled to create a python byte generator.
    return buf.bytes()

def device_raw(device=None):
    """Raw saleae byte sequence."""
    if device == None:
        device = pyla.devices()[0]
    buf = pyla.memory()
    device.connect_sink(buf)
    return buf.bytes()

# Backward compat
saleae_with = device_with
saleae_raw = device_raw



# MULTIPLE OUTPUTS (not abstracted as streams)
# FIXME: not sure yet how to present this in API...

# For the most common case of running a couple of analyzers in
# parallel, hide the more general dataflow Forth API.
def parallel(procs):
 

    # Create a program by concatenation.  Each method call appends an
    # operation to the program.
    p = pyla.stack_program()
    for proc in procs:
        p.dup()     # duplicate input
        p.op(proc)  # process input -> output
        p.save()    # push result to save stack
    p.drop()        # get rid of input

    # When a program is used in a stack_op_sink, the program is
    # presented with a stack containing one buffer: the current sample
    # chunk.  After the program is finished, the output corresponds to
    # the contents of the operand stack and the save stack, back to
    # back.  E.g. with 2 and 3 elements remaining, there are 5 outputs:
    #
    # operand  save
    # 1 top    top 1  2   // data on stacks
    # 4  3      2  1  0   // mapped to outputs
    #
    # In short, data appears in the output in the same order it is
    # passed to the `save` operation.
    return p


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
