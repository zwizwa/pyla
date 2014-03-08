%module pylacore
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%{
#include "pylacore.h"
#include "uart.h"
#include "syncser.h"
#include "saleae.h"
#include "compose.h"
#include "rpn.h"
#include "shared.h"
%}


%include "boost_shared_ptr.i"
%shared_ptr(frontend)
%shared_ptr(operation)
%shared_ptr(uart)
%shared_ptr(syncser)
%shared_ptr(dedup)
%shared_ptr(sink)
%shared_ptr(source)
%shared_ptr(buffer)
%shared_ptr(hole)
%shared_ptr(memory)
%shared_ptr(memmap)
%shared_ptr(compose_op_op)
%shared_ptr(compose_op_src)
%shared_ptr(compose_snk_op)
%shared_ptr(stack_op)
%shared_ptr(stack_manip)
%shared_ptr(stack_compute)
%shared_ptr(stack_program)
%shared_ptr(stack_op_sink)
%shared_ptr(chunk_stack)

%include "pylacore.h"
%include "uart.h"
%include "syncser.h"
%include "compose.h"
%include "rpn.h"
%include "shared.h"
%include "saleae.h"


%template(chunk) std::vector<unsigned char>;
%shared_ptr(chunk)



%template(salea_device_list) std::vector<saleae *>;


