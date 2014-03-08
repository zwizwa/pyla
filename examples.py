from pyla import *
from stream import *

# DUMP SPI as ASCII

def saleae_spi():
    return saleae_with(
        syncser(),
        record="/tmp/spi.bin",
        config={
            'clock_channel'  : 0,
            'data_channel'   : 1,
            'frame_channel'  : 2,
            'frame_active'   : 0,  # SPI /CS  
            'clock_edge'     : 1,  # sampling edge: 0->1
            'clock_polarity' : 0,  # clock idle polarity
        })

def saleae_uart():
    return saleae_with(
        uart(),
        config={
            'channel'  : 3,
            'baudrate' : 115200,
        })


    

print_hex(saleae_spi())
# print_ascii(saleae_uart())


