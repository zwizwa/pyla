from pyla import *
from stream import *


## Currently samplerate can only be set once at startup.
# pyla.saleae.start(4000000)
# pyla.saleae.start(24000000)
pyla.saleae.start(12000000)

def saleae_spi():
    return saleae_with(
        syncser().config({
            'clock_channel'  : 0,
            'data_channel'   : 1,
            'frame_channel'  : 2,
            'frame_active'   : 0,  # SPI /CS  
            'clock_edge'     : 1,  # sampling edge: 0->1
            'clock_polarity' : 0,  # clock idle polarity
        }),
        record = "/tmp/spi.bin")


    
print_hex(saleae_spi())


