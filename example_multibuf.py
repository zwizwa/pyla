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
    
    # Create dataflow program.  Before each run, the saleae data will
    # be loaded on the stack.  The data that remains on the operand
    # stack + save stack determines the output vector.
    _ = pyla.stack_program()

    _.dup()
    _.op(_spi)  
    _.save()    # output 0
    _.op(_uart) 
    _.save()    # output 1   (last save is redundant)

    # Wrap program as a buffered sink
    mb = multibuf(_)

    # Connect to the sampling device
    mb.connect(devices()[0])

    # Print both streams to console.
    while True:
        buf = mb.read_multi()
        print_hex  (buf[0])
        print_ascii(buf[1])
        time.sleep(0.1)


print_uart_and_spi()


