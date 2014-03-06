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

/* INTERFACES */

/* All data formats are untyped byte streams.  KISS.  This is a debug
   tool.  Let's assume you know what your data means. */
typedef std::vector<unsigned char> chunk;

#if 0 // not a good idea because of operator& used in memcpy
class vector_chunk {
 public:
  vector_chunk(uint64_t size) : _v(std::vector<unsigned char>(size)) {}
  vector_chunk() : _v(std::vector<unsigned char>()) {}
  unsigned char operator[](uint64_t i) { return _v[i]; }

  bool empty() { return _v.empty(); }
  void clear() { return _v.clear(); }
  uint64_t size() { return _v.size(); }

 private:
  std::vector<unsigned char> _v;
};
#endif


/* Externally triggered data processor. */
class operation {
 public:
  virtual void process(chunk&, chunk&) = 0;
  virtual ~operation() {}
};

/* Source is the pull interface for data producers. */
class source {
 public:
  virtual void read(chunk&) = 0;
  virtual ~source() {}
};

/* Sink is the push interface for data consumers. */
class sink {
 public:
  virtual void write(chunk&) = 0;
  virtual ~sink() {}
};

/* Buffer write should never fail.  Overrun == fatal error.
   Read and write should be threadsafe.
   Buffer read is non-blocking, returning empty vector when buffer is empty.
   Read chunk size is implementation-dependent. */
class buffer : public source, public sink {
 public:
  virtual void read(chunk&) = 0;
  virtual void write(chunk&) = 0;
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
  void read(chunk&);
  void write(chunk&);
  ~hole();
};

// Temporary buffer in memory
class memory : public buffer { 
 public:
  memory();
  void read(chunk&);
  void write(chunk&);
  ~memory();
 private:
  std::list<chunk> _buf;
  mutex _mutex;
};

// File-backed circular buffer
class file : public buffer { 
 public:
  file(const char *filename, uint64_t size);
  void read(chunk&);
  void write(chunk&);
  void clear();
  ~file();
 private:
  FILE *_store;
  uint8_t *_buf;
  uint64_t _read_index;
  uint64_t _write_index;
  uint64_t _size;
};






#endif // _PYLACORE_H


