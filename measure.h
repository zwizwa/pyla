#ifndef _MEASURE_H
#define _MEASURE_H

#include "pylacore.h"

class frequency : public frontend {
 public:
  frequency();
  void set_samplerate(double sr);
  void process(chunk &out, chunk &in);
  void reset();
};


#endif
