
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

class analyzer {
public:
  /* KISS: byte vector to byte vector conversion. */
  virtual std::vector<unsigned char> process(std::vector<unsigned char>) = 0;
  virtual ~analyzer() {}
};

class frontend : public analyzer {
public:
  virtual void set_samplerate(double sr) = 0;
  virtual void reset() = 0;
  /* Parse a chunk of 8 channel binary data at samplerate, return a
     chunk of parsed bytes (representation not specified). */
  virtual std::vector<unsigned char> process(std::vector<unsigned char>) = 0;
  virtual ~frontend() {}
};

/* Source is the pull interface for data producers.
   Sources can be made by chaining an analyzer and a source. 
   FIXME: DAGs
*/
class source {
 public:
  virtual std::vector<unsigned char> read() = 0;
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
  virtual std::vector<unsigned char> read() = 0;
  virtual void write(std::vector<unsigned char>) = 0;
  virtual ~buffer() {}
};



/* TOOLS */
class chain : public source {
 public:
  chain(analyzer *a, source *s) {
    _a = a;
    _s = s;
  }
  std::vector<unsigned char> read() {
    std::vector<unsigned char> output = _a->process(_s->read());
    while(1) {
      /* Poll until empty.  This allows the implementation to avoid
         merging vectors, by returning one vector at a time. */
      std::vector<unsigned char> chunk = _a->process(_s->read());
      if (!chunk.size()) break;
      output.insert(output.end(), chunk.begin(), chunk.end());
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
  std::vector<unsigned char> read();
  void write(std::vector<unsigned char>);
  ~blackhole();
};

class memory : public buffer { 
 public:
  memory();
  std::vector<unsigned char> read();
  void write(std::vector<unsigned char>);
  ~memory();
 private:
  std::list<std::vector<unsigned char> > _buf;
  mutex _mutex;
};


#endif // _PYLACORE_H
