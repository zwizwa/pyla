#ifndef _PYLACORE_H
#define _PYLACORE_H

#define PYLA_DEFAULT_SAMPLERATE 12000000

#if 1
#include <stdio.h>
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...)
#endif

#include <vector>
#include <list>
#include <memory>
#include <mutex.h>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

/* INTERFACES */

/* All data formats are untyped byte streams.  KISS.  This is a debug
   tool.  Let's assume you know what your data means. 

   Regarding memory management:
   - process doesn't manage memory; uses references
   - read/write use shared pointers
*/
typedef std::vector<unsigned char> chunk;


/* Note that operation, source, sink has a different API in python,
   C++ uses in-place operation (references),  f (in, out)
   Python uses a   out = f ( in ) style */

/* Externally triggered data processor. */
class operation {
 public:
  virtual void process(chunk& output, chunk& input) = 0;
  virtual ~operation() {}
};

/* Source is the pull interface for data producers. */
class source {
 public:
  virtual boost::shared_ptr<chunk> read() = 0;
  virtual ~source() {}

  chunk read_copy() {
    return *read();
  }
};

/* Sink is the push interface for data consumers. */
class sink {
 public:
  virtual void write(boost::shared_ptr<chunk>) = 0;
  virtual ~sink() {}

  void write_copy(chunk& c) {
    boost::shared_ptr<chunk> p = boost::shared_ptr<chunk>(new chunk(c));
    write(p);
  }

};

/* Buffer write should never fail.  Overrun == fatal error.
   Read and write should be threadsafe.
   Buffer read is non-blocking, returning empty vector when buffer is empty.
   Read chunk size is implementation-dependent. */
class buffer : public source, public sink {
 public:
  virtual boost::shared_ptr<chunk> read() = 0;
  virtual void write(boost::shared_ptr<chunk>) = 0;
  virtual ~buffer() {}

};






/* A sampler is something that has a samplerate associated to its data stream.
   set_samplerate_hint() asks for a samplerate near the requested rate.
   get_samplerate() is exact. */
class sampler {
 public:
  virtual double get_samplerate() = 0;
  virtual void set_samplerate_hint(double sr) = 0;
  virtual ~sampler() {}
};





/* a fancier operation */
class frontend : public operation {
public:
  virtual void set_samplerate(double sr) = 0;
  virtual void reset() = 0;
  virtual void process(chunk&, chunk&) = 0;
  virtual ~frontend() {}
};






/* TOOLS */

// Everything disappears
class hole : public buffer {
 public:
  boost::shared_ptr<chunk> read();
  void write(boost::shared_ptr<chunk>);
  ~hole();
};

// Temporary buffer in memory with optional logfile.
class memory : public buffer { 
 public:
  memory();
  boost::shared_ptr<chunk> read();
  void write(boost::shared_ptr<chunk>);
  void set_log(const char *filename);
  ~memory();
 private:
  std::list<boost::shared_ptr<chunk> > _buf;
  mutex _mutex;
  FILE *_log;
};

// File-backed circular buffer
class memmap : public buffer { 
 public:
  memmap(const char *filename, uint64_t size);
  boost::shared_ptr<chunk> read();
  void write(boost::shared_ptr<chunk>);
  void clear();
  ~memmap();
 private:
  FILE *_store;
  uint8_t *_buf;
  uint64_t _read_index;
  uint64_t _write_index;
  uint64_t _size;
};







#endif // _PYLACORE_H


