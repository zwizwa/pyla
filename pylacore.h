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
class buffer : public source, public sink {
 public:
  virtual void read(chunk&) = 0;
  virtual void write(chunk&) = 0;
  virtual ~buffer() {}
};


/* COMPOSITION */
class compose_op_src : public source {
 public:
  compose_op_src(operation *a, source *s) : _a(a), _s(s) { }
  ~compose_op_src() {}  // FIXME: ownership?
  void read(chunk& output) {
    chunk input;
    _s->read(input);
    while(!input.empty()) {
      _a->process(output, input);
      input.clear();
      _s->read(input);
    }
  }
 private:
  operation *_a;
  source *_s;
};

class compose_snk_op : public sink {
 public:
  compose_snk_op(sink *s, operation *a) : _a(a), _s(s) { }
  ~compose_snk_op() {}  // FIXME: ownership?
  void write(chunk& input) {
    chunk output;
    _a->process(output, input);
    _s->write(output);
  }
 private:
  operation *_a;
  sink *_s;
};

class compose_op_op : public operation {
 public:
  compose_op_op(operation *a, operation *b) : _a(a), _b(b) { }
  ~compose_op_op() {}  // FIXME: ownership?
  void process(chunk& output, chunk& input) {
    chunk tmp;
    _a->process(tmp, input);
    _b->process(output, tmp);
  }
 private:
  operation *_a;
  operation *_b;
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
