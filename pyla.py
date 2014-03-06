import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")
sys.path.append(".")
sys.path.append("build")

import pylacore
import time
import re


# classes


# Plug through all shared_ wrappers
for method in dir(pylacore):
    match = re.match("shared_(.*)", method)
    g = globals()
    if match:
        name = match.group(1)
        shared_name = match.group(0)
        # print("pyla.%s = pylacore.%s" % (name, shared_name))
        g[name] = getattr(pylacore, shared_name)


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

