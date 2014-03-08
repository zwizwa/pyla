#ifndef _COMPOSE_H
#define _COMPOSE_H

#include "pylacore.h"

#include <boost/shared_ptr.hpp>
#include <list>

// FIXME: remove after finishing rpn.h

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




#endif // _COMPOSE_H

