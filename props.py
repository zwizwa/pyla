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

def a_tuple(*el_types):
    while True:
        yield tuple([t.__next__() for t in el_types])

def forall(n, f, *arg_types):
    for i,args in enumerate(a_tuple(*arg_types)):
        f(*args)
        if i == n:
            return

if __name__ == '__main__':
    def prnt(*args):
        print(args)
    forall(10, prnt, a_byte, a_byte)
    forall(10, prnt, a_list(a_byte))
