#include "pylacore.h"
#include <iostream>

/* Wrapper functions for Python to work around pass-by-reference. */
chunk process(operation *op, chunk input) {
  chunk output;
  op->process(output, input);
  return output;
}
chunk read(source *src) {
  chunk output;
  src->read(output);
  return output;
}


/* Composition */
compose_op_src :: compose_op_src(operation* op, source* src) : _op(op), _src(src) { }
compose_op_src :: ~compose_op_src() { LOG("~compose_op_src()\n"); }
void compose_op_src :: read(chunk& output) {
  chunk input;
  _src->read(input);
  while(!input.empty()) {
    _op->process(output, input);
    input.clear();
    _src->read(input);
  }
}

compose_snk_op :: compose_snk_op(sink* snk, operation* op) : _op(op), _snk(snk) { }
compose_snk_op :: ~compose_snk_op() { LOG("~compose_snk_op()\n"); }
void compose_snk_op :: write(chunk& input) {
  chunk output;
  _op->process(output, input);
  if (!output.empty()) {
    _snk->write(output);
  }
}

compose_op_op :: compose_op_op(operation* op1, operation* op2) : _op1(op1), _op2(op2) { }
compose_op_op :: ~compose_op_op() { LOG("~compose_op_op()\n"); }
void compose_op_op :: process(chunk& output, chunk& input) {
  chunk tmp;
  _op1->process(tmp, input);
  _op1->process(output, tmp);
}




/* Buffers */

void blackhole::read(chunk& output) {
  chunk empty; 
  output = empty;
}
void blackhole::write(chunk& input) {
  std::cerr << "drop: " << input.size() << std::endl;
}
blackhole::~blackhole() {
  LOG("~blackhole()\n");
}


memory::memory() {
}
void memory::write(chunk& input) {
  _mutex.lock();
  if (!input.empty()) {
    _buf.push_back(input);
  }
  _mutex.unlock();
}
void memory::read(chunk& output) {
   _mutex.lock();
   
   if (_buf.empty()) {
     chunk empty;
     output = empty;
   }
   else {
     output = _buf.front();
     _buf.pop_front();
   }
   _mutex.unlock();
}

memory::~memory() {
}



