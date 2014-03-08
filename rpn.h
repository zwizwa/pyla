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
  : _stack(std::list<boost::shared_ptr<chunk> >()),
    _save_stack(std::list<boost::shared_ptr<chunk> >()) {}


  /* Data i/o */
  boost::shared_ptr<chunk> top() {
    return _stack.front();
  }
  boost::shared_ptr<chunk> pop() {
    boost::shared_ptr<chunk> c = top();
    _stack.pop_front();
    return c;
  }
  void push(boost::shared_ptr<chunk> c) { 
    _stack.push_front(c); 
  }

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
    _save_stack.push_front(pop());
  }
  void load() {
    push(_save_stack.front());
    _save_stack.pop_front();
  }
  void clear() {
    while (!_stack.empty()) { drop(); }
  }

 private:
  std::list<boost::shared_ptr<chunk> > _stack;
  std::list<boost::shared_ptr<chunk> > _save_stack;
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
  void compile(boost::shared_ptr<operation> op) { _program.push_back(new stack_compute(op)); }
  void compile(void(chunk_stack::*op)())        { _program.push_back(new stack_manip(op)); }
  /* Run program */
  void run(chunk_stack& s) {
    for (std::list<stack_op*>::const_iterator p = _program.begin(), end = _program.end();
         p != end;
         ++p) {
      (*p)->run(s);
    }
  }
  /* Cleanup */
  void clear() {
    while (!_program.empty()) {
      delete(_program.front());
      _program.pop_front();
    }
  }
  ~stack_program() { clear(); }
 private:
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
  void write(chunk& input) {
    _stack->clear();
    // FIXME: avoid copy
    boost::shared_ptr<chunk> c = boost::shared_ptr<chunk>(new chunk(input));
    _stack->push(c);
    _program->run(*_stack);
  }

 private:
  boost::shared_ptr<stack_op>    _program;
  boost::shared_ptr<chunk_stack> _stack;
};






#endif // _RPN_H
