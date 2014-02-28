#ifndef _UART_H
#define _UART_H
#include "pylacore.h"
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
  virtual std::vector<unsigned char> process(std::vector<unsigned char>);  
  virtual void reset();
  void set_samplerate(double sr);
  void set_baudrate(double br);
  void set_channel(int c);
 private:
  void _update_clock_div();
  void _set_state(enum sm_state s);

  /* Bitstream config. */
  double _samplerate;
  unsigned int _clock_div; // _sr / _br
  int _channel;

  /* UART config */
  double _baudrate;
  unsigned char _bit_stop;   // bit index of stop and parity bits
  unsigned char _bit_parity;

  /* Bit-level parser state. */
  enum sm_state _state;      // current state
  unsigned int  _delay;      // clock ticks to next action
  unsigned char _bits_data;
  unsigned char _bits_count;
  unsigned char _bits_parity;

};

#endif // _UART_H
