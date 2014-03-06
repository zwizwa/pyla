/* Shared object wrapping.  These are separate to keep the leaf
   classes clean for reuse. */

#ifndef _FACTORY_H
#define _FACTORY_H

#include "pylacore.h"
#include "uart.h"
#include "syncser.h"

#include <boost/shared_ptr.hpp>
#include <list>


/* A data aquisition device or DAQ abstracts a physical device
   implemented as a system callback / ISR.  It can deliver data to a
   sink, and is therefore called a co-sink.

   It is not the same as a source.  A source is a callable pull object
   - not a callback framework.  A source can be made by combining a
   DAQ and a buffer. 

   This is part of shared.h as there will be references from Python as
   well.

*/

class cosink {
 public:
  virtual void connect_sink(boost::shared_ptr<sink>) = 0;
};

/* There is no cosource - the thing that reads a source.
   That would be your Python code. */



/* Compositions use shared pointers to enforce the use of shared
   factory methods instead of raw constructors from python.  For each
   composition constructor, also provide a boost::shared_ptr wrapped version.
 */
class compose_snk_op : public sink {
 public:
  compose_snk_op(boost::shared_ptr<sink> snk,
                 boost::shared_ptr<operation> op)
    : _op(op), _snk(snk) { }
  ~compose_snk_op() {  LOG("~compose_snk_op()\n"); }
  void write(chunk& input) {
    chunk output;
    _op->process(output, input);
    if (!output.empty()) {
      _snk->write(output);
    }
  }
 private:
  boost::shared_ptr<operation> _op;
  boost::shared_ptr<sink> _snk;
};

class compose_op_src : public source {
 public:
 compose_op_src(boost::shared_ptr<operation> op,
                boost::shared_ptr<source> src) 
    : _op(op), _src(src) { }
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
  boost::shared_ptr<operation> _op;
  boost::shared_ptr<source> _src;
};

class compose_op_op : public operation {
 public:
  compose_op_op(boost::shared_ptr<operation> op1,
                boost::shared_ptr<operation> op2)
    : _op1(op1), _op2(op2) { }
  ~compose_op_op() { LOG("~compose_op_op()\n"); }
  void process(chunk& output, chunk& input) {
    chunk tmp;
    _op1->process(tmp, input);
    _op1->process(output, tmp);
  }
 private:
  boost::shared_ptr<operation> _op1;
  boost::shared_ptr<operation> _op2;
};


/* Buffer 2-stack. */
template<class T>
class twostack {
 public:
 twostack() 
  : _stack(std::list<boost::shared_ptr<T> >()),
    _save_stack(std::list<boost::shared_ptr<T> >()) {}
  void drop() { _stack.pop_front(); }
  boost::shared_ptr<T> pop() { boost::shared_ptr<T> c = top(); drop(); return c; }
  boost::shared_ptr<T> top() { return _stack.front(); }
  void push(boost::shared_ptr<T> c) { _stack.push_front(c); }
  void dup() { push(top()); }

  void save() { _save_stack.push_front(pop()); }
  void load() { push(_save_stack.front()); _save_stack.pop_front(); }
 private:
  std::list<boost::shared_ptr<T> > _stack;
  std::list<boost::shared_ptr<T> > _save_stack;
};

typedef twostack<chunk> chunkstack ;



/* Wrapped constructors */

// 0-arg constructors are all the same pattern so use macro
#define wrap(cls) \
  static inline boost::shared_ptr<cls> make_shared_##cls() { \
    return boost::shared_ptr<cls>(new cls());           \
  }
wrap(uart)
wrap(syncser)
wrap(memory)
wrap(diff)
wrap(hole)
wrap(chunk)
wrap(chunkstack)


// multi-arg constructors are written out
static inline boost::shared_ptr<chunk>
make_shared_chunk(uint64_t size) {
  return boost::shared_ptr<chunk>(new chunk(size));
}

static inline boost::shared_ptr<memmap>
make_shared_file(const char *filename, uint64_t size) {
  return boost::shared_ptr<memmap>(new memmap(filename, size));
}

static inline boost::shared_ptr<compose_snk_op>
make_shared_compose_snk_op(boost::shared_ptr<sink> snk,
                           boost::shared_ptr<operation> op) {
  return boost::shared_ptr<compose_snk_op>(new compose_snk_op(snk, op));
}
static inline boost::shared_ptr<compose_op_op>
make_shared_compose_op_op(boost::shared_ptr<operation> op1,
                          boost::shared_ptr<operation> op2) {
  return boost::shared_ptr<compose_op_op>(new compose_op_op(op1, op2));
}
static inline boost::shared_ptr<compose_op_src>
make_shared_compose_op_src(boost::shared_ptr<operation> op,
                           boost::shared_ptr<source> src) {
  return boost::shared_ptr<compose_op_src>(new compose_op_src(op, src));
}





/* These copy to/from vector<unsigned char> to make interop with
   python arrays possible.
   FIXME: Find a way to use shared_ptr<chunk> */
static inline std::vector<unsigned char>
copy_process(operation *op, std::vector<unsigned char> input) {
  std::vector<unsigned char> output;
  op->process(output, input);
  return output;
}
static inline std::vector<unsigned char>
copy_read(source *src) {
  std::vector<unsigned char> output;
  src->read(output);
  return output;
}
static inline void
copy_write(sink* snk, std::vector<unsigned char> input) {
  snk->write(input);
}





#endif
