from pyla import *
from stream import *
import time

## Currently samplerate can only be set once at startup.

pyla.saleae.start(24000000)


def print_uart_and_spi():
    # Instantiate and configure analyzers
    _spi_i = syncser().config({
        'clock_channel'  : 0,
        'data_channel'   : 1,
        'clock_edge'     : 1,  # sampling edge: 0->1
        'clock_polarity' : 0,  # clock idle polarity
    })
    _spi_o = syncser().config({
        'clock_channel'  : 0,
        'data_channel'   : 2,
        'clock_edge'     : 1,  # sampling edge: 0->1
        'clock_polarity' : 0,  # clock idle polarity
    })
    
    # Create dataflow program.  See implementation of `parallel' in
    # stream.py for an example of Forth-like dataflow composition.
    p = parallel([_spi_i, _spi_o])

    # Wrap program as a buffered sink
    mb = multibuf(p)

    # Connect to the sampling device
    mb.connect(devices()[0])

    # Print both streams to console.
    while True:
        [buf_i, buf_o] = mb.read_multi()
        if buf_i:
            print_hex(buf_i, newline="\nI:\n")
        if buf_o:
            print_hex(buf_o, newline="\nO:\n")
        time.sleep(0.1)


print_uart_and_spi()


