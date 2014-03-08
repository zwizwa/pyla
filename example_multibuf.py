from pyla import *
from stream import *
import time

## Currently samplerate can only be set once at startup.
# pyla.saleae.start(4000000)


def print_uart_and_spi():
    # Instantiate and configure analyzers
    _uart = uart().config({
        'channel'  : 3,
        'baudrate' : 115200,
    })
    _spi = syncser().config({
        'clock_channel'  : 0,
        'data_channel'   : 1,
        'frame_channel'  : 2,
        'frame_active'   : 0,  # SPI /CS  
        'clock_edge'     : 1,  # sampling edge: 0->1
        'clock_polarity' : 0,  # clock idle polarity
    })
    
    # Create dataflow program.  See implementation of `parallel' in
    # stream.py for an example of Forth-like dataflow composition.
    p = parallel([_spi, _uart])

    # Wrap program as a buffered sink
    mb = multibuf(p)

    # Connect to the sampling device
    mb.connect(devices()[0])

    # Print both streams to console.
    while True:
        [buf_spi, buf_ascii] = mb.read_multi()
        print_hex  (buf_spi)
        print_ascii(buf_ascii)
        time.sleep(0.1)


print_uart_and_spi()


