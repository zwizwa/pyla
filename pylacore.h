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
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
typedef boost::mutex mutex;


/* INTERFACES */

/* All data formats are untyped byte streams.  KISS.  This is a debug
   tool.  Let's assume you know what your data means. 

   Regarding memory management:
   - operation doesn't manage memory; uses references
   - read/write use shared pointers  (+ optional _copy() methods)
*/
typedef std::vector<unsigned char> chunk;


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
  chunk read_copy() { return *read(); }
  virtual ~source() {}
};

/* Sink is the push interface for data consumers. */
class sink {
 public:
  virtual void write(boost::shared_ptr<chunk>) = 0;
  void write_copy(chunk& c) { write(boost::shared_ptr<chunk>(new chunk(c))); }
  virtual ~sink() {}
};

/* Buffer write should never fail.  Overrun == fatal error.
   Read and write should be threadsafe.
   Buffer read is non-blocking, returning empty vector when buffer is empty.
   Read chunk size is implementation-dependent. */
class buffer : public source, public sink {
 public:
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
  virtual ~frontend() {}
};


/* A data aquisition device or DAQ abstracts a physical device
   implemented as a system callback / ISR.  It can deliver data to a
   sink, and is therefore called a co-sink.

   It is not the same as a source.  A source is a callable pull object
   - not a callback framework.  A source can be made by combining a
   DAQ and a buffer. 

   This is part of shared.h as there will be references from Python as
   well. */

class cosink {
 public:
  virtual void connect_sink(boost::shared_ptr<sink>) = 0;
};

/* There is no cosource - the thing that reads a source.
   That would be your Python code. */




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


