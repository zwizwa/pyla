#include "syncser.h"

syncser::syncser() {
  _clock_channel  = 0;
  _data_channel   = 1;
  _frame_channel  = -1; // disabled
  _clock_edge     = 1;  // postive edge triggering
  _clock_polarity = 0;
  _frame_active   = 0;
  reset();
};

void syncser::reset() {
  _clock_state = _clock_polarity;
  _frame_state = _frame_active ^ 1;
  _shift_count = 0;
  _shift_reg = 0;
}

void syncser::process(chunk& output, chunk& input) {
  int i, i_size = input.size();
  for (i=0; i<i_size; i++) {
    int clock_bit = (input[i] >> _clock_channel) & 1;
    int frame_bit = (input[i] >> _frame_channel) & 1;
    int data_bit  = (input[i] >> _data_channel) & 1;

    /* Frame edge */
    // FIXME: this should wait to do anything if it starts in the
    // middle of a frame.
    if (_frame_channel >= 0) { // framing enabled
      if (frame_bit != _frame_state) { // transition
        if (frame_bit == _frame_active) {
          // reset shift register
          _shift_reg = 0;
          _shift_count = 0;
        }
      }
    }

    /* Shift in data on sampling clock edge. */
    if ((_frame_channel < 0) | // ignore framing or
        (frame_bit == _frame_active)) { // frame active
      if (clock_bit != _clock_state) {  // transition
        if (clock_bit == _clock_edge) { // sampling edge
          _shift_reg <<= 1; // (A) 
          _shift_reg |= data_bit;
          _shift_count++;
          if (_shift_count == 8) { // (B)
            output.push_back(_shift_reg);
            // reset shift register
            _shift_reg = 0;
            _shift_count = 0;
          }
        }
      }
    }

    /* Edge detector state */
    _clock_state = clock_bit;
    _frame_state = frame_bit;
  }
}

/* (A) Is it necessary to provide a LSBit first shift?  Both SPI and
       I2C seem to use MSBit first in all cases I've encountered.

   (B) For word-oriented streams, it might be good to shift in full
       words, then allow endianness config in the output stream.
   
*/



dedup::dedup() : _last(-1) {}

void dedup::process(chunk& output, chunk& input) {
  int i, i_size = input.size();
  for (i=0; i<i_size; i++) {
    unsigned char b = input[i];
    if (b != _last) {
      output.push_back(b);
      _last = b;
    }
  }
}
