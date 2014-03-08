#!/usr/bin/python3.3 -i
import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")

import pylacore
import time
import re



# Here the make_shared_ wrapped C++ constructors get wrapped again to
# produce objects with additional functionality.

class io_wrapper:
    def __init__(self, ob):
        self.core = ob

    # Override to present a simpler API in Python.
    #       process(in,out)  =>  out = process(in)
    #       read(in)         =>  in  = read()
    # 
    # The original methods are still available as:
    #       .core.process(o, i)
    #       .core.read(i)
    #       .core.write(i)

    def process(self, inbuf):
        # FIXME: array to chunk& conversion seems to need an
        # intermediate step going through a copy.
        i = pylacore.chunk(inbuf) 
        o = pylacore.chunk()
        self.core.process(o, i)
        return o
    def read(self):
        o = pylacore.chunk()
        self.core.read(o)
        return o
    def write(self, inbuf):
        i = pylacore.chunk(inbuf) 
        self.core.write(i)

    # Add some extra functionality.
    def bytes(self):
        return buf_gen(self.core)



    # Delegate rest: behave as a subclass.
    def __getattr__(self, attr):
        return getattr(self.core, attr)

    
def io_wrapper_factory(cons):
    def new_cons(*args):
        ob = cons(*args)
        # FIXME: do not wrap objects without process/read/write interface
        return io_wrapper(ob)
    return new_cons



# The shared_xyz functions are wrappers around the xyz base objects,
# creating boost::shared_ptr to allow proper memory management for
# objects shared between Python and C++ code (callbacks, composition,
# ...).
#   pyla.uart = pylacore.make_shared_uart 
#   etc...

for attrib in dir(pylacore):
    match = re.match("shared_(.*)", attrib)
    pyla = globals()
    if match:
        dst_name = match.group(1)
        src_name = match.group(0)
        print("pyla.%s = pylacore.%s" % (dst_name, src_name))
        # patch pyla. method to wrapped shared factory
        pyla[dst_name] = io_wrapper_factory(getattr(pylacore, src_name))


_poll = []
def register_poll(method):
    _poll.append(method)

# special
def devices():
    devices = []
    while not devices: # Wait for connection
        time.sleep(0.1)
        devices = pylacore.saleae.devices()
    return devices

# FIXME: later use condition variables
def read_blocking(buf):
    while 1:
        for method in _poll:
            method()
        out = bytes(buf.read())
        if (len(out)):
            return out
        else:
            time.sleep(.04)


def buf_gen(buf):
    """Convert pyla buffer to python byte generator."""
    while 1:
        for b in read_blocking(buf):
            yield(b)
