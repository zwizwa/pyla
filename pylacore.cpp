#include "pylacore.h"

pylacore::pylacore()
{
}

std::vector<unsigned char> pylacore::helloworld() {
  std::vector<unsigned char> rv;
  rv.push_back(1);
  rv.push_back(2);
  rv.push_back(3);
  return rv;
}
