%module pylacore
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%{
#include "pylacore.h"
#include "uart.h"
#include "syncser.h"
#include "saleae.h"
#include "factory.h"
%}
%include "boost_shared_ptr.i"
%shared_ptr(frontend)
%shared_ptr(operation)
%shared_ptr(uart)
%shared_ptr(syncser)
%shared_ptr(diff)
%shared_ptr(compose_op_op)

namespace std {
  %template(UnsignedIntVector) std::vector<unsigned int>;
  %template(UnsignedCharVector) std::vector<unsigned char>;
  %template(SaleaePointerVector) std::vector<saleae *>;
}

%include "pylacore.h"
%include "uart.h"
%include "syncser.h"
%include "saleae.h"
%include "factory.h"

