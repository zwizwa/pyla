%module pylacore
%include "std_string.i"
%include "std_vector.i"
%{
#include "pylacore.h"
#include "uart.h"
#include "syncser.h"
#include "saleae.h"
%}

namespace std {
  %template(UnsignedIntVector) std::vector<unsigned int>;
  %template(UnsignedCharVector) std::vector<unsigned char>;
  %template(SaleaePointerVector) std::vector<saleae *>;
}

%include "pylacore.h"
%include "uart.h"
%include "syncser.h"
%include "saleae.h"

