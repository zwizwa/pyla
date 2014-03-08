/* RPN composition. */

#ifndef _RPN_H
#define _RPN_H

#include "pylacore.h"

#include <boost/shared_ptr.hpp>
#include <list>



/* Buffer 2-stack. */
class chunk_stack {
 public:
 chunk_stack() 
  : _operands(std::list<boost::shared_ptr<chunk> >()),
    _save    (std::list<boost::shared_ptr<chunk> >()) {}


  /* Data i/o */
  boost::shared_ptr<chunk> top() {
    return _operands.front();
  }
  boost::shared_ptr<chunk> pop() {
    boost::shared_ptr<chunk> c = top();
    _operands.pop_front();
    return c;
  }
  void push(boost::shared_ptr<chunk> c) { 
    _operands.push_front(c); 
  }
  bool empty() { return _operands.empty(); }
  int size() { return _operands.size(); }

  chunk top_copy() {
    // FIXME: Something is wrong with swig wrapping of shared pointers..
    chunk c = *top();
    return c;
  }

  /* The void (*)() methods can be used in chunk_program. */

  void push() {
    boost::shared_ptr<chunk> c = boost::shared_ptr<chunk>(new chunk());
    push(c);
  }
  void drop() {
    pop();
  }
  void dup() {
    push(top());
  }
  void save() {
    _save.push_front(pop());
  }
  void load() {
    push(_save.front());
    _save.pop_front();
  }
  void clear() {
    while (!_operands.empty()) { drop(); }
    while (!_save.empty()) { _save.pop_front(); }
  }

 private:
  std::list<boost::shared_ptr<chunk> > _operands;
  std::list<boost::shared_ptr<chunk> > _save;
};

/* Perform operation on stack.
   stack_op      : interface
   stack_manip   : dup, drop, ...
   stack_program : sequence of stack_op  (i.e. a dataflow Forth program)
*/
class stack_op {
 public:
  virtual void run(chunk_stack&) = 0;
  virtual ~stack_op() {}
};
class stack_manip : public stack_op {
 public:
 stack_manip(void(chunk_stack::*op)()) : _op(op) {}
  void run(chunk_stack& s) { (s.*_op)(); }
 private:
  void(chunk_stack::*_op)();
};
class stack_compute : public stack_op {
 public:
  stack_compute(boost::shared_ptr<operation> op) : _op(op) {}
  void run(chunk_stack& s) {
    boost::shared_ptr<chunk> i = s.pop();
    s.push();
    boost::shared_ptr<chunk> o = s.top();
    _op->process(*o, *i);
  }
 private:
  boost::shared_ptr<operation> _op;
};
class stack_program : public stack_op {
 public:
  /* Construct a program. */
  void op(boost::shared_ptr<operation> op) {
    _program.push_back(new stack_compute(op));
  }
  void dup()  { _op(&chunk_stack::dup);  }
  void drop() { _op(&chunk_stack::drop); }
  void load() { _op(&chunk_stack::load); }
  void save() { _op(&chunk_stack::save); }

  /* Run program */
  void run(chunk_stack& s) {
    for (std::list<stack_op*>::const_iterator
           p   = _program.begin(),
           end = _program.end();
         p != end;
         ++p) {
      (*p)->run(s);
    }
  }
  /* Cleanup */
  ~stack_program() { 
    while (!_program.empty()) {
      delete(_program.front());
      _program.pop_front();
    }
  }
 private:
  void _op(void(chunk_stack::*op)()) {
    _program.push_back(new stack_manip(op));
  }
  std::list<stack_op*> _program;
};


/* Present a stack_op operation as a sink object. */
class stack_op_sink : public sink {
 public:
  stack_op_sink(boost::shared_ptr<stack_op> program,
                boost::shared_ptr<chunk_stack> stack) :
    _program(program),
    _stack(stack) { }

  /* Sink interface. */
  void write(boost::shared_ptr<chunk> input) {
    _stack->clear();
    _stack->push(input);
    _program->run(*_stack);
  }

 private:
  boost::shared_ptr<stack_op>    _program;
  boost::shared_ptr<chunk_stack> _stack;
};






#endif // _RPN_H
