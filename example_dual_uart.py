from pyla import *
from stream import *
import time

## Currently samplerate can only be set once at startup.

pyla.saleae.start(12000000)


def print_uart_and_spi():
    # Instantiate and configure analyzers
    _uart_tx = uart().config({
        'channel'  : 3,
        'baudrate' : 115200,
    })
    _uart_rx = uart().config({
        'channel'  : 4,
        'baudrate' : 115200,
    })
    
    # Create dataflow program.  See implementation of `parallel' in
    # stream.py for an example of Forth-like dataflow composition.
    p = parallel([_uart_tx, _uart_rx])

    # Wrap program as a buffered sink
    mb = multibuf(p)

    # Connect to the sampling device
    mb.connect(devices()[0])

    # Print both streams to console.
    while True:
        [buf_tx, buf_rx] = mb.read_multi()
        if buf_tx:
            print_hex(buf_tx, newline="\ntx:\n")
        if buf_rx:
            print_hex(buf_rx, newline="\nrx:\n")
        time.sleep(0.1)


print_uart_and_spi()


