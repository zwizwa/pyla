#ifndef _PYLACORE_H
#define _PYLACORE_H

#include <string>
#include <vector>

#include "frontend.h"

class pylacore {
 public:
  pylacore();
  std::string identify();
  void load(std::string name);
  std::vector<unsigned char> analyze(std::vector<unsigned char>);
 private:
  void _clear_frontend();
  frontend *_frontend;
  
};

#endif // _PYLACORE_H

