from pyla import *
from stream import *

## Use SPI as debugging output.  Useful for projects that already have
## the UART(s) used for something else.  SPI on the master side is
## very easy to bit-bang.


# pyla.saleae.start(16000000)

pyla.saleae.start(8000000)

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


print_ascii(saleae_spi())


