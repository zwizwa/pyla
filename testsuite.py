import sys
if sys.version_info[0] != 3:
    raise NameError("Python version 3 needed.")
sys.path.append("build")
import pylacore

# Core module doesn't have any functionality.  Should this be removed?
core = pylacore.pylacore()
# print(core.identify())


def oversample(lst,n):
    for e in lst:
        for i in range(n):
            yield(e)

# Bit generators
# Note that SPI is MSB first while UART is LSB first.
def number_bits(number, nb):
    for i in range(nb):
        yield (number >> i) & 1

def bytes_bits(bs):
    for c in bs:
        for b in number_bits(c, 8):
            yield b

def parity(bits):
    p = 0
    for b in bits:
        p ^= b
    return p

def uart_frame(byte):
    l = list(number_bits(byte, 8))
    return [0] + l + [parity(l)] + [1]

def check(cond, msg):
    if not cond:
        raise NameError(msg)
    print("PASS: %s" % msg)

# UART
def test_uart():
    uart = pylacore.uart()
    br = 9600
    ov = 16
    sr = br * ov
    uart.set_baudrate(br)
    uart.set_samplerate(sr)
    uart.set_channel(0)

    inbyte = 0x0F
    idle = [1,1,1]
    bits = idle + uart_frame(inbyte) + idle

    ov_bytes = bytes(oversample(bits, ov))
    # print(ov_bytes)
    output = uart.analyze(ov_bytes)
    # print(bytes(output))
    check(output[0] == inbyte, "uart byte %02X" % inbyte)

test_uart()

                               
