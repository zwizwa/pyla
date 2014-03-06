
# Bit generators
# Note that SPI is MSB first while UART is LSB first.
def number_bits(number, nb):
    for i in range(nb):
        yield (number >> i) & 1

def bytes_bits(bs):
    for c in bs:
        for b in number_bits(c, 8):
            yield b

def oversample(lst,n):
    for e in lst:
        for i in range(n):
            yield(e)

def parity(bits):
    p = 0
    for b in bits:
        p ^= b
    return p

def uart_frame_par(byte):
    l = list(number_bits(byte, 8))
    return [0] + l + [parity(l)] + [1]

def uart_frame_nopar(byte):
    l = list(number_bits(byte, 8))
    return [0] + l + [1]

