#include "pylacore.h"
#include <iostream>


chunk blackhole::read() {
  chunk empty; return empty;
}
void blackhole::write(chunk input) {
  std::cerr << "drop: " << input.size() << std::endl;
}
blackhole::~blackhole() {
}


// This is naive: eliminate copying by using shared pointers.
memory::memory() {
}
void memory::write( chunk input) {
  _mutex.lock();
  if (!input.empty()) {
    _buf.push_back(input);
  }
  _mutex.unlock();
}
 chunk memory::read() {
   _mutex.lock();
   chunk vec;
   if (!_buf.empty()) {
     vec = _buf.front();
     _buf.pop_front();
   }
   _mutex.unlock();
   return vec;
}

memory::~memory() {
}
