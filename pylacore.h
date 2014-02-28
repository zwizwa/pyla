
#ifndef _PYLACORE_H
#define _PYLACORE_H

#include <vector>


/* INTERFACES */

class analyzer {
public:
  /* KISS: byte vector to byte vector conversion. */
  virtual std::vector<unsigned char> analyze(std::vector<unsigned char>) = 0;
  //virtual ~analyzer() = 0;
};

class frontend : public analyzer {
public:
  virtual void set_samplerate(double sr) = 0;
  virtual void reset() = 0;
  /* Parse a chunk of 8 channel binary data at samplerate, return a
     chunk of parsed bytes (representation not specified). */
  virtual std::vector<unsigned char> analyze(std::vector<unsigned char>) = 0;
  //virtual ~frontend() = 0;
};

/* Read is non-blocking, returning empty vector when buffer is empty.
   Blocking read uses explicit sync.
   After sync returns, read() returns non-empty vector.
   set_samplerate_hint() asks for a samplerate near the requested rate.
   get_samplerate() is exact. */
class sampler {
 public:
  virtual std::vector<unsigned char> read() = 0;
  virtual void read_sync() = 0;
  virtual double get_samplerate() = 0;
  virtual void set_samplerate_hint(double sr) = 0;
  //virtual ~sampler() = 0;
};

/* Buffer write should never fail.  Overrun == fatal error.
   Buffer read is non-blocking, returning empty vector when buffer is empty.
   Read chunk size is implementation-dependent.
   Blocking read uses explicit sync.
   After sync returns, read() returns non-empty vector. */
class buffer {
 public:
  virtual std::vector<unsigned char> read() = 0;
  virtual void write(std::vector<unsigned char>) = 0;
  virtual void read_sync() = 0;
  virtual ~buffer() {}
};





#endif // _PYLACORE_H
