%module pylacore
%include "std_string.i"
%include "std_vector.i"
%{
#include "pylacore.h"
#include "uart.h"
%}
 
namespace std {
  %template(UnsignedIntVector) std::vector<unsigned int>;
  %template(UnsignedCharVector) std::vector<unsigned char>;
}

%include "pylacore.h"
%include "frontend.h"
%include "uart.h"

