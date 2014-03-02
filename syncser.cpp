#include "syncser.h"

syncser::syncser() {
  _clock_channel = 0;
  _data_channel  = 1;
  _frame_channel = -1; // disabled
  _clock_edge    = 1;  // postive edge triggering
  reset();
};

void syncser::reset() {
  _clock_state = 0; // FIXME: hardcoded at clock polarity 0
  _frame_state = 1; // (C)
  _shift_count = 0;
  _shift_reg = 0;
}

void syncser::process(chunk& output, chunk& input) {
  int i, i_size = input.size();
  for (i=0; i<i_size; i++) {
    if (_frame_channel >= 0) {
      int frame_bit = (input[i] >> _frame_channel) & 1;
      if (frame_bit != _frame_state) {
        // reset on transition
        _shift_reg = 0;
        _shift_count = 0;
        _frame_state = frame_bit;
      }
      // ignore clock / data unless frame bit asserted
      if (frame_bit != 0) continue;
    }

    int clock_bit = (input[i] >> _clock_channel) & 1;
    int data_bit  = (input[i] >> _data_channel) & 1;
    if (clock_bit != _clock_state) {
      if (clock_bit == _clock_edge) {
        _shift_reg <<= 1; // (A) 
        _shift_reg |= data_bit;
        _shift_count++;
        if (_shift_count == 8) { // (B)
          output.push_back(_shift_reg);
          _shift_reg = 0;
          _shift_count = 0;
        }
      }
      _clock_state = clock_bit;
    }
  }
}

/* (A) Is it necessary to provide a LSBit first shift?  Both SPI and
       I2C seem to use MSBit first in all cases I've encountered.

   (B) For word-oriented streams, it might be good to shift in full
       words, then allow endianness config in the output stream.
   
   (C) Frame sync / chip select hardcoded at active low.
*/



diff::diff() : _last(-1) {}

void diff::process(chunk& output, chunk& input) {
  int i, i_size = input.size();
  for (i=0; i<i_size; i++) {
    unsigned char b = input[i];
    if (b != _last) {
      output.push_back(b);
      _last = b;
    }
  }
}
