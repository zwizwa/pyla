#ifndef _PYLACORE_H
#define _PYLACORE_H

#include <vector>
#include <list>
#include <mutex.h>

/* INTERFACES
   
   All interfaces use pull polling, where chained objects poll
   recursively.  Pulling seems more natural that pushing from a
   device callback.

*/

typedef std::vector<unsigned char> chunk;

class analyzer {
 public:
  /* KISS: byte vector to byte vector conversion. */
  virtual chunk process(chunk) = 0;
  virtual ~analyzer() {}
};

class frontend : public analyzer {
public:
  virtual void set_samplerate(double sr) = 0;
  virtual void reset() = 0;
  /* Parse a chunk of 8 channel binary data at samplerate, return a
     chunk of parsed bytes (representation not specified). */
  virtual chunk process(chunk) = 0;
  virtual ~frontend() {}
};

/* Source is the pull interface for data producers.
   Sources can be made by chaining an analyzer and a source. 
   FIXME: DAGs
*/
class source {
 public:
  virtual chunk read() = 0;
  virtual ~source() {}
};

/* Read is non-blocking, returning empty vector when buffer is empty.
   set_samplerate_hint() asks for a samplerate near the requested rate.
   get_samplerate() is exact. */
class sampler : public source {
 public:
  virtual double get_samplerate() = 0;
  virtual void set_samplerate_hint(double sr) = 0;
  virtual ~sampler() {}
};

/* Buffer write should never fail.  Overrun == fatal error.
   Read and write should be threadsafe.
   Buffer read is non-blocking, returning empty vector when buffer is empty.
   Read chunk size is implementation-dependent. */
class buffer : public source {
 public:
  virtual chunk read() = 0;
  virtual void write(chunk) = 0;
  virtual ~buffer() {}
};



/* TOOLS */
class chain : public source {
 public:
  chain(analyzer *a, source *s) {
    _a = a;
    _s = s;
  }
  chunk read() {
    chunk output;
    chunk input = _s->read();
    while(!input.empty()) {
      chunk chunk = _a->process(input);
      output.insert(output.end(), chunk.begin(), chunk.end());
      input = _s->read();
    }
    return output;
  }
  ~chain() {}  // FIXME: ownership?
 private:
  analyzer *_a;
  source *_s;
};

class blackhole : public buffer {
 public:
  chunk read();
  void write(chunk);
  ~blackhole();
};

class memory : public buffer { 
 public:
  memory();
  chunk read();
  void write(chunk);
  ~memory();
 private:
  std::list<chunk> _buf;
  mutex _mutex;
};


#endif // _PYLACORE_H
