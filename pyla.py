import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")
sys.path.append(".")
sys.path.append("build")

import pylacore
import time
import re



# Here the make_shared_ wrapped C++ constructors get wrapped again to
# produce objects with additional functionality.

class io_wrapper:
    def __init__(self, ob):
        self._ob = ob

    # Just override process and read to present a different API in
    # python.  E.g.  process(in,out)  =>  out = process(in)
    def process(self, inbuf):
        return pylacore.copy_process(self._ob, inbuf)
    def read(self):
        return pylacore.copy_read(self._ob)
    def write(self, inbuf):
        pylacore.copy_write(self._ob, inbuf)

    # Add some extra functionality.
    def bytes(self):
        return buf_gen(self._ob)



    # Delegate rest: behave as a subclass.
    def __getattr__(self, attr):
        return getattr(self._ob, attr)

    
def io_wrapper_factory(cons):
    def new_cons(*args):
        return io_wrapper(cons(*args))
    return new_cons



# The make_shared_ functions are wrappers around the base objects,
# creating boost::shared_ptr to allow proper memory management for
# objects shared between Python and C++ code (callbacks, composition,
# ...).
#   pyla.uart = pylacore.make_shared_uart 
#   etc...

for attrib in dir(pylacore):
    match = re.match("make_shared_(.*)", attrib)
    pyla = globals()
    if match:
        dst_name = match.group(1)
        src_name = match.group(0)
        print("pyla.%s = pylacore.%s" % (dst_name, src_name))
        # patch pyla. method to wrapped shared factory
        pyla[dst_name] = io_wrapper_factory(getattr(pylacore, src_name))



# disable constructor
pylacore.saleae = None


_poll = []
def register_poll(method):
    _poll.append(method)

# special
def devices():
    devices = []
    while not devices: # Wait for connection
        time.sleep(0.1)
        devices = pylacore.saleae.devices()
    for d in devices:
        r.register(d)
    return devices

# FIXME: later use condition variables
def read_blocking(buf):
    while 1:
        for method in _poll:
            method()
        out = bytes(pylacore.read(buf))
        if (len(out)):
            return out
        else:
            time.sleep(.04)


def buf_gen(buf):
    """Convert pyla buffer to python byte generator."""
    while 1:
        for b in pyla.read_blocking(buf):
            yield(b)
