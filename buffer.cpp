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

void blackhole::read(chunk& output) {
  chunk empty; 
  output = empty;
}
void blackhole::write(chunk& input) {
  std::cerr << "drop: " << input.size() << std::endl;
}
blackhole::~blackhole() {
}


// This is naive: eliminate copying by using shared pointers.
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
