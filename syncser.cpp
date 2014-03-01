#include "syncser.h"

syncser::syncser() {
  _clock_channel = 0;
  _data_channel = 1;
  _clock_edge = 1;
  reset();
};

void syncser::reset() {
  _shift_count = 0;
  _shift_reg = 0;
  _clock_state = 0;
};

  
void syncser::process(chunk& output, chunk& input) {
  int i, i_size = input.size();
  for (i=0; i<i_size; i++) {
    int clock_bit = (input[i] >> _clock_channel) & 1;
    int data_bit  = (input[i] >> _data_channel) & 1;
    if (clock_bit != _clock_state) {
      if (clock_bit == _clock_edge) {
        _shift_reg <<= 1;
        _shift_reg |= data_bit;
        _shift_count++;
        if (_shift_count == 8) {
          output.push_back(_shift_reg);
          _shift_reg = 0;
          _shift_count = 0;
        }
      }
      _clock_state = clock_bit;
    }
  }
}
