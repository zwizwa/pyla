/* Shared object wrapping.  These are separate to keep the leaf
   classes clean for reuse. */

#ifndef _FACTORY_H
#define _FACTORY_H

#include "pylacore.h"
#include "uart.h"
#include "syncser.h"

#include <boost/shared_ptr.hpp>

/* Compositions use shared pointers to enforce the use of shared
   factory methods instead of raw constructors from python.  For each
   composition constructor, also provide a shared_ptr wrapped version.
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


// 0-arg constructors are all the same pattern so use macro
#define wrap(cls) \
  static inline boost::shared_ptr<cls> shared_##cls() { \
    return boost::shared_ptr<cls>(new cls());           \
  }
wrap(uart)
wrap(syncser)
wrap(memory)
wrap(diff)
wrap(hole)


// multi-arg constructors are written out
static inline boost::shared_ptr<file>
shared_file(const char *filename, uint64_t size) {
  return boost::shared_ptr<file>(new file(filename, size));
}

static inline boost::shared_ptr<compose_snk_op>
shared_compose_snk_op(boost::shared_ptr<sink> snk,
                      boost::shared_ptr<operation> op) {
  return boost::shared_ptr<compose_snk_op>(new compose_snk_op(snk, op));
}
static inline boost::shared_ptr<compose_op_op>
shared_compose_op_op(boost::shared_ptr<operation> op1,
                     boost::shared_ptr<operation> op2) {
  return boost::shared_ptr<compose_op_op>(new compose_op_op(op1, op2));
}
static inline boost::shared_ptr<compose_op_src>
shared_compose_op_src(boost::shared_ptr<operation> op,
                      boost::shared_ptr<source> src) {
  return boost::shared_ptr<compose_op_src>(new compose_op_src(op, src));
}



#endif
