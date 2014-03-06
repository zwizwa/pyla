import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")
sys.path.append(".")
sys.path.append("build")

import pylacore
import time
import re


# classes


# For each object constructed, allow some patching.  The C++ code uses
# in-place operations on chunk references.  Python API uses a
# functional y = f(x) API.


class io_wrapper:
    def __init__(self, cons):
        self._cons = cons

    def __call__(self, *args):
        ob = self._cons(*args)
        return ob


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
        name = match.group(1)
        shared_name = match.group(0)
        print("pyla.%s = pylacore.%s" % (name, shared_name))
        # patch pyla. method to shared factory
        cons = getattr(pylacore, shared_name)
        pyla[name] = io_wrapper(cons)




# functions
process = pylacore.process
read    = pylacore.read
write   = pylacore.write

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

