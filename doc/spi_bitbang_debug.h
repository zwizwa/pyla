/* Bit-banged serial output is connected to the ICSP pins:
   PGED1 = RB0  (PIN4 on 28-pin DIP dsPIC33FJ128GP802)
   PGEC1 = RB1  (PIN5)

   Using the pyla syncser analyzer on ICSP pins is a very
   straightforward way to have a debug output port without messing
   with clock frequencies. */

// Put your component here.
#include <p33FJ128GP804.h>   

// I/O pins
#define debug_data       LATBbits.LATB0
#define debug_clock      LATBbits.LATB1
#define debug_frame      LATBbits.LATB2

#define debug_tris_data  TRISBbits.TRISB0
#define debug_tris_clock TRISBbits.TRISB1
#define debug_tris_frame TRISBbits.TRISB2

#define DEBUG_DELAY 0 // 100

#if !defined(DEBUG_FRAMED)
#define DEBUG_FRAMED 1
#endif

static inline void debug_init(void) {
    debug_data = 0;
    debug_clock = 0; // idle clock before switching on output
    debug_tris_data = 0;
    debug_tris_clock = 0;
    if (DEBUG_FRAMED) {
        debug_frame = 1;
        debug_tris_frame = 0;
    }
}
static inline void debug_delay(void) {
    volatile int delay = DEBUG_DELAY;
    while(delay--);
}
static inline void debug_putbit(int bit) {
    debug_data = bit;
    debug_delay();
    debug_clock = 1; // sample edge
    debug_delay();
    debug_clock = 0; // return to idle
}
static inline void debug_set_frame(int bit) {
    if (DEBUG_FRAMED) {
        debug_frame = bit;
        debug_delay();
    }
}
static inline void debug_putchar(unsigned char c) {
    debug_set_frame(0);
    int shift = 8;
    while(shift--) { // MSBit first
        debug_putbit(c >> shift);
    }
    debug_set_frame(1);
}
static inline void debug_cr_lf() {
    debug_putchar('\n');
    debug_putchar('\r');
}
static inline void debug_puts(const char *c) {
    while(*c) {debug_putchar(*c++);}
    debug_cr_lf();
}

const unsigned char hextable[] = "0123456789ABCDEF";
static inline void debug_puthex_digit(unsigned char c) {
  debug_putchar(hextable[c&0xF]);
}
static inline void debug_puthex(unsigned char c) {
  debug_puthex_digit(c >> 4);
  debug_puthex_digit(c);
}
static inline void debug_hexdump(unsigned char *buf, int size) {
  int i;
  for(i=0; i<size; i++) {
    unsigned char *c = buf + i;
    if(0 == (i & 0xF)) {
      debug_puthex(((unsigned int)c) >> 8);
      debug_puthex(((unsigned int)c) >> 0);
      debug_putchar(' ');
    }
    debug_puthex(*c);
    debug_putchar(' ');
    if(0xF == (i & 0xF)) debug_cr_lf();
  }
}
