#ifndef _SYNCSER_H
#define _SYNCSER_H

#include "pylacore.h"

/* General purpose clock + N syncronous serial protocol. */

class syncser : public frontend {
 public:
  syncser();
  virtual void process(chunk&, chunk&);
  virtual void reset();
  void set_samplerate(double sr) {} // clocked, so not necessary

  void set_clock_edge(int e)     { _clock_edge = e; }
  void set_clock_channel(int c)  { _clock_channel = c; }
  void set_clock_polarity(int p) { _clock_polarity = p; }
  void set_data_channel(int c)   { _data_channel = c; }
  void set_frame_channel(int c)  { _frame_channel = c; }
  void set_frame_active(int a)   { _frame_active = a; }

 private:
  /* config */
  int _clock_channel;
  int _data_channel;
  int _frame_channel;
  int _clock_edge;
  int _clock_polarity;
  int _frame_active;

  /* state */
  unsigned int _shift_count;
  unsigned int _shift_reg;
  int _clock_state;
  int _frame_state;
};


/* Leave only changes in the output stream.  Useful for writing
   synchronous protocols in python as it lowers CPU usage
   significantly. */

class diff : public operation {
 public:
  diff();
  void process(chunk&, chunk&);
 private:
  int _last;
};

#endif //_SYNCSER_H


