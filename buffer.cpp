#include "pylacore.h"
#include <iostream>


std::vector<unsigned char> blackhole::read() {
  std::vector<unsigned char> empty; return empty;
}
void blackhole::write(std::vector<unsigned char> input) {
  std::cerr << "drop: " << input.size() << std::endl;
}
blackhole::~blackhole() {
}


// This is naive: eliminate copying by using shared pointers.
memory::memory() {
}
void memory::write( std::vector<unsigned char> input) {
  _mutex.lock();
  if (!input.empty()) {
    _buf.push_back(input);
  }
  _mutex.unlock();
}
 std::vector<unsigned char> memory::read() {
   _mutex.lock();
   std::vector<unsigned char> vec;
   if (!_buf.empty()) {
     vec = _buf.front();
     _buf.pop_front();
   }
   _mutex.unlock();
   return vec;
}

memory::~memory() {
}
