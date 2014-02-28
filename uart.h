#ifndef _UART_H
#define _UART_H
#include "frontend.h"
#include <string>
#include <stdint.h>

enum sm_state {
  sm_idle   = 1,
  sm_sample = 2,
  sm_break  = 3,
};

class uart : public frontend {
 public:
  uart();
  virtual std::vector<unsigned char> analyze(std::vector<unsigned char>);  
  virtual void reset();
  void set_samplerate(double sr);
  void set_baudrate(double br);
  void set_channel(int c);
 private:
  void _update_clock_div();
  double _br;
  double _sr;
  unsigned int _clock_div; // _sr / _br
  int _channel;
  void _log(std::string s) {}
  void _set_state(enum sm_state s) { 
    _state = s;
    switch(_state) {
    case sm_idle:   _log("I"); break;
    case sm_sample: _log("S"); break;
    case sm_break:  _log("B"); break;
    }
  }

  /* Bit-level parser state. */
  enum sm_state _state;      // current state
  unsigned int  _delay;      // clock ticks to next action
  unsigned char _bits_data;
  unsigned char _bits_count;
  unsigned char _bits_parity;

};

#endif // _UART_H
