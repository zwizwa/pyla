#!/usr/bin/python3.3 -i
import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")

import pylacore
import time
import re

# FIXME.  Use condition variables and multiple threads instead?
_poll = []
def register_poll(method):
    _poll.append(method)
def poll():
    for method in _poll:
        method()

# Here the make_shared_ wrapped C++ constructors get wrapped again to
# produce objects with additional functionality.

class io_wrapper:
    def __init__(self, ob):
        self.core = ob
    def __getattr__(self, attr):
        return getattr(self.core, attr)

class process_wrapper(io_wrapper):
    def __init__(self, ob):
        super(process_wrapper, self).__init__(ob)
    def process(self, inbuf):
        i = pylacore.chunk(inbuf)  # copy to keep swig happy
        o = pylacore.chunk()       # we need to alloc output
        self.core.process(o, i)
        return o
    def config(self, c):
        apply_config(self.core, c)
        return self

class buffer_wrapper(io_wrapper):
    def __init__(self, ob):
        super(buffer_wrapper, self).__init__(ob)

    # Use chunk copies to keep swig happy.
    def write(self, inbuf):
        self.core.write_copy(pylacore.chunk(inbuf))
    def read(self):
        return self.core.read_copy()

    # Extra functionality.
    def read_blocking(self):
        while 1:
            poll() # FIXME
            out = bytes(self.core.read_copy())
            if (len(out)):
                return out
            else:
                time.sleep(.04)
    def bytes(self):
        """Convert pyla buffer to python byte generator."""
        while 1:
            for b in self.read_blocking():
                yield(b)



def maybe_attrs(ob, attrs):
    lst = []
    for attr in attrs:
        try:
            lst.append(getattr(ob, attr))
        except:
            pass
    return lst
    
def io_wrapper_factory(cons):
    def new_cons(*args):
        ob = cons(*args)
        if 0 < len(maybe_attrs(ob, ['read','write'])):
            return buffer_wrapper(ob)
        if 0 < len(maybe_attrs(ob, ['process'])):
            return process_wrapper(ob)
        return ob
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



# special
def devices():
    devices = []
    while not devices: # Wait for connection
        time.sleep(0.1)
        devices = pylacore.saleae.devices()
    return devices

def apply_config(obj, config):
    """Apply config dict as set_ methods."""
    print("config: %s" % obj)
    if config:
        for (key, val) in config.items():
            try:
                print("config: %s=%d" % (key, val))
                getattr(obj, 'set_' + key)(val)
            except Exception as e:
                print("WARNING: set_%s not defined: %s" % (key, e))
    return obj


class program():
    def __init__(self):
        self._p = stack_program()
    def __getattr__(self, attr):
        m = getattr(self._p, attr)
        def comp(*args):
            m(*args)
            return self
        return comp

    
