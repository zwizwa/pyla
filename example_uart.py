from pyla import *
from stream import *


## Currently samplerate can only be set once at startup.
# pyla.saleae.start(24000000)
pyla.saleae.start(4000000)

def saleae_uart():
    return saleae_with(
        uart().config({
            'channel'  : 3,
            'baudrate' : 115200,
        }))

# print_ascii(saleae_uart())
print_console(saleae_uart())


