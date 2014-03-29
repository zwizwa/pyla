import random
import itertools

# Generators for property based testing.
# A "type" is represented by a generator of random values.

# Different kinds of random distrubitions are not yet supported - use
# separate "types".

def a_range(min, max):
    while True:
        yield random.randint(min, max)

a_byte = a_range(0, 255)

def a_list(el_type, min=0, max=10):
    while True:
        len = random.randint(min, max)
        yield [el_type.__next__() for i in range(len)]

def a_tuple(el_types):
    while True:
        yield tuple([t.__next__() for t in el_types])

def mapgen(n, f, *arg_types):
    i = 0
    for args in a_tuple(arg_types):
        f(*args)
        i += 1
        if i == n:
            return


if __name__ == '__main__':
    def prnt(*args):
        print(args)
    mapgen(10, prnt, a_byte, a_byte)
    mapgen(10, prnt, a_list(a_byte))
