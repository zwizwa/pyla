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

/* INTERFACES
   
   All interfaces use pull polling, where chained objects poll
   recursively.  Pulling seems more natural that pushing from a
   device callback.

*/

typedef std::vector<unsigned char> chunk;

 


class operation {
 public:
  /* KISS: byte vector to byte vector conversion. */
  virtual void process(chunk&, chunk&) = 0;
  virtual ~operation() {}
};

class frontend : public operation {
public:
  virtual void set_samplerate(double sr) = 0;
  virtual void reset() = 0;
  /* Parse a chunk of 8 channel binary data at samplerate, return a
     chunk of parsed bytes (representation not specified). */
  virtual void process(chunk&, chunk&) = 0;
  virtual ~frontend() {}
};

/* Source is the pull interface for data producers.
   Sources can be made by chaining an operation and a source. 
   FIXME: DAGs
*/
class source {
 public:
  virtual void read(chunk&) = 0;
  virtual ~source() {}
};

class sink {
 public:
  virtual void write(chunk&) = 0;
  virtual ~sink() {}
};

/* set_samplerate_hint() asks for a samplerate near the requested rate.
   get_samplerate() is exact. */
class sampler {
 public:
  virtual double get_samplerate() = 0;
  virtual void set_samplerate_hint(double sr) = 0;
  virtual ~sampler() {}
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




/* COMPOSITION */
class compose_op_src : public source {
 public:
  compose_op_src(operation* op, source* src) : _op(op), _src(src) { }
  ~compose_op_src() { LOG("~compose_op_src()\n"); }
  void read(chunk& output) {
    chunk input;
    _src->read(input);
    while(!input.empty()) {
      _op->process(output, input);
      input.clear();
      _src->read(input);
    }
  }
 private:
  operation* _op;
  source* _src;
};

class compose_snk_op : public sink {
 public:
  compose_snk_op(sink* snk, operation* op) : _op(op), _snk(snk) { }
  ~compose_snk_op() { LOG("~compose_snk_op()\n"); }
  void write(chunk& input) {
    chunk output;
    _op->process(output, input);
    if (!output.empty()) {
      _snk->write(output);
    }
  }
 private:
  operation* _op;
  sink* _snk;
};

class compose_op_op : public operation {
 public:
  compose_op_op(operation* op1, operation* op2) : _op1(op1), _op2(op2) { }
  ~compose_op_op() { LOG("~compose_op_op()\n"); }
  void process(chunk& output, chunk& input) {
    chunk tmp;
    _op1->process(tmp, input);
    _op1->process(output, tmp);
  }
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

/* Wrapper functions for Python to work around pass-by-reference. */
chunk process(operation *, chunk);
chunk read(source *);
 


#endif // _PYLACORE_H
