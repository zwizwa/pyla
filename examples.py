from pyla import *
from stream import *

# DUMP SPI as ASCII

def dump_spi_hex():
    """SPI HEX data logger."""
    print_hex(
        saleae_with(
            syncser(),
            config={
                'clock_channel'  : 0,
                'data_channel'   : 1,
                'frame_channel'  : 2,
                'frame_active'   : 0,  # SPI /CS  
                'clock_edge'     : 1,  # sampling edge: 0->1
                'clock_polarity' : 0,  # clock idle polarity
            }))

dump_spi_hex()

# test_saleae_syncser()
# dump_diff()

# dump_syncser(frame=2,dump=print_hex)



