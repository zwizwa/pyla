%module pylacore
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%{
#include "pylacore.h"
#include "uart.h"
#include "syncser.h"
#include "saleae.h"
#include "shared.h"
%}
%include "boost_shared_ptr.i"
%shared_ptr(frontend)
%shared_ptr(operation)
%shared_ptr(uart)
%shared_ptr(syncser)
%shared_ptr(diff)
%shared_ptr(sink)
%shared_ptr(source)
%shared_ptr(buffer)
%shared_ptr(hole)
%shared_ptr(memory)
%shared_ptr(file)
%shared_ptr(compose_op_op)
%shared_ptr(compose_op_src)
%shared_ptr(compose_snk_op)
%shared_ptr(chunkstack)
%shared_ptr(chunk)

namespace std {
  %template(UnsignedIntVector) std::vector<unsigned int>;
  %template(UnsignedCharVector) std::vector<unsigned char>;
  %template(SaleaePointerVector) std::vector<saleae *>;
  %template(SharedChunk) boost::shared_ptr<std::vector<unsigned char> >;
}

%include "pylacore.h"
%include "uart.h"
%include "syncser.h"
%include "shared.h"
%include "saleae.h"

