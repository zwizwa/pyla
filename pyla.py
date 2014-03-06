import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")
sys.path.append(".")
sys.path.append("build")

import pylacore
import time


# classes

# shared wrappers
syncser        = pylacore.shared_syncser
uart           = pylacore.shared_uart
diff           = pylacore.shared_diff
blackhole      = pylacore.shared_blackhole
memory         = pylacore.shared_memory
file           = pylacore.shared_file

compose_snk_op = pylacore.shared_compose_snk_op
#compose_op_src = pylacore.shared_compose_op_src
#compose_op_op  = pylacore.shared_compose_op_op


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
        out = bytes(pylacore.read(buf))
        if (len(out)):
            return out
        else:
            time.sleep(.04)

