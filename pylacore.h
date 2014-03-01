#ifndef _PYLACORE_H
#define _PYLACORE_H

#if 1
#include <stdio.h>
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...)
#endif

#include <vector>
#include <list>
#include <mutex.h>

/* INTERFACES */

/* All data formats are untyped byte streams.  KISS.  This is a debug
   tool.  Let's assume you know what your data means. */
typedef std::vector<unsigned char> chunk;


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




/* A data aquisition device or DAQ abstracts a physical device
   implemented as a system callback / ISR.  It can deliver data to a
   sink, and is therefore called a co-sink.

   It is not the same as a source.  A source is a callable pull object
   - not a callback framework.  A source can be made by combining a
   DAQ and a buffer. */

class cosink {
 public:
  virtual void connect_sink(sink*) = 0;
};

/* There is no cosource - the thing that reads a source.
   That would be your Python code. */



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





/* COMPOSITION */
class compose_op_src : public source {
 public:
  compose_op_src(operation* op, source* src);
  ~compose_op_src();
  void read(chunk& output);
 private:
  operation* _op;
  source* _src;
};

class compose_snk_op : public sink {
 public:
  compose_snk_op(sink* snk, operation* op);
  ~compose_snk_op();
  void write(chunk& input);
 private:
  operation* _op;
  sink* _snk;
};

class compose_op_op : public operation {
 public:
  compose_op_op(operation* op1, operation* op2);
  ~compose_op_op();
  void process(chunk& output, chunk& input);
 private:
  operation* _op1;
  operation* _op2;
};

/* TOOLS */
class blackhole : public buffer {
 public:
  void read(chunk&);
  void write(chunk&);
  ~blackhole();
};

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

/* Wrapper functions for Python to work around pass-by-reference for
   the chunk type. */
chunk process(operation *, chunk);
chunk read(source *);

#endif // _PYLACORE_H
