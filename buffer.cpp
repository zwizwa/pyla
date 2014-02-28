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
  _buf.push_back(input);
}
 std::vector<unsigned char> memory::read() {
  if (_buf.empty()) {
    std::vector<unsigned char> empty;
    return empty;
  }
  else {
     std::vector<unsigned char> first =_buf.front();
    _buf.pop_front();
    return first;
  }
}

memory::~memory() {
}
