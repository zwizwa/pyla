#include "pylacore.h"
#include "uart.h"

/* Not sure if this module is necessary.  Maybe rename to "network"
   and allow arbitrary DAG composition of analyzers. */

pylacore::pylacore()
{
}

std::string pylacore::identify() {
  return "pylacore";
}
