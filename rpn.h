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
  void load_all() {
    while (!_save.empty()) { load(); }
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


/* Present a stack_op operation as a sink object.  Data gets pushed
   into the write() method, which triggers the program to run.  The
   result is then gathered from the stack and pushed into the output
   sinks.  */
class stack_op_sink : public sink {
 public:
 stack_op_sink(boost::shared_ptr<stack_op> program) :
    _program(program) { 
      /* Perform a dummy run to determine the number of outputs. */
      boost::shared_ptr<chunk> probe = boost::shared_ptr<chunk>(new chunk());
      _run(probe);
      /* Initialize output vector with holes. */
      for (int i = 0; i<_stack.size(); i++) {
        boost::shared_ptr<hole> h = boost::shared_ptr<hole>(new hole());
        _outputs.push_back(h);
      }
    }
  void connect_output(unsigned int index, boost::shared_ptr<sink> s) { 
    if (index < _outputs.size()) _outputs[index] = s;
  }
  void write(boost::shared_ptr<chunk> input) {
    _run(input);
    _move_to_outputs();
  }
  int nb_outputs() {
    return _outputs.size();
  }
  
 private:
  void _run(boost::shared_ptr<chunk> input) {
    _stack.clear();
    _stack.push(input);
    _program->run(_stack);
    _stack.load_all(); // simplifies programs
  }
  void _move_to_outputs() {
    int index = 0;
    while(!_stack.empty()) {
      boost::shared_ptr<chunk> c = _stack.pop();
      if (index < _outputs.size()) _outputs[index]->write(c);
      index++;
    }
  }
  chunk_stack _stack;
  boost::shared_ptr<stack_op> _program;
  std::vector<boost::shared_ptr<sink> > _outputs;
};






#endif // _RPN_H
