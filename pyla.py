import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")
sys.path.append(".")
sys.path.append("build")

import pylacore
import time
import atexit


# FIXME: Some memory management is needed for objects accessible both
# in python and the co-sink objects in C++ (callbacks).  For now each
# object is simply registered forever (leaked) using these wrapper
# functions.

# To reset, disconnect everything from the saleae co-sink objects and
# clear the registry.

class registry:
    def __init__(self):
        self.objects = []
    def register(self, ob):
        self.objects.append(ob)
        return ob
    def wrap(self, method):
        print(method)
        def proxy(*args):
            return self.register(method(*args))
        return proxy
    def disconnect_sinks(self):
        pylacore.saleae.disconnect_sinks()
        

r = registry()


# Avoid crashes on teardown / DAQ thread is still going strong.
atexit.register(r.disconnect_sinks)



# classes
syncser        = r.wrap(pylacore.syncser)
uart           = r.wrap(pylacore.uart)
diff           = r.wrap(pylacore.diff)
blackhole      = r.wrap(pylacore.blackhole)
memory         = r.wrap(pylacore.memory)
file           = r.wrap(pylacore.file)
compose_snk_op = r.wrap(pylacore.compose_snk_op)
compose_op_src = r.wrap(pylacore.compose_op_src)
compose_op_op  = r.wrap(pylacore.compose_op_op)

# functions
process = pylacore.process
read    = pylacore.read


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
        out = bytes(pylacore.read(buf, 128*1024))
        if (len(out)):
            return out
        else:
            time.sleep(.04)

