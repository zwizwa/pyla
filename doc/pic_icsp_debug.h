/* Bit-banged serial output is connected to the ICSP pins:
   PGED1 = RB0  (PIN4 on 28-pin DIP dsPIC33FJ128GP802)
   PGEC1 = RB1  (PIN5)

   Using the pyla syncser analyzer on ICSP pins is a very
   straightforward way to have a debug output port without messing
   with clock frequencies. */

// Put your component here.
#include <p33FJ128GP802.h>   

// I/O pins
#define debug_data       LATBbits.LATB0
#define debug_clock      LATBbits.LATB1
#define debug_tris_data  TRISBbits.TRISB0
#define debug_tris_clock TRISBbits.TRISB1

static inline void debug_init(void) {
    debug_data = 0;
    debug_clock = 0; // idle clock before switching on output
    debug_tris_data = 0;
    debug_tris_clock = 0;
}
static inline void debug_delay(void) {
    volatile int delay = 100;
    while(delay--);
}
static inline void debug_putbit(int bit) {
    debug_data = bit;
    debug_delay();
    debug_clock = 1; // sample edge
    debug_delay();
    debug_clock = 0; // return to idle
}
static inline void debug_putchar(unsigned char c) {
    int shift = 8;
    while(shift--) { // MSBit first
        debug_putbit(c >> shift);
    }
}
static inline void debug_puts(const char *c) {
    while(*c) {debug_putchar(*c++);}
    debug_putchar('\n');
    debug_putchar('\r');
}
