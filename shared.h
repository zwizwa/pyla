/* Shared object wrapping.  These are separate to keep the leaf
   classes clean for reuse. */

#ifndef _SHARED_H
#define _SHARED_H

#include "pylacore.h"
#include "uart.h"
#include "syncser.h"
#include "compose.h"
#include "rpn.h"


#include <boost/shared_ptr.hpp>
#include <list>


/* A data aquisition device or DAQ abstracts a physical device
   implemented as a system callback / ISR.  It can deliver data to a
   sink, and is therefore called a co-sink.

   It is not the same as a source.  A source is a callable pull object
   - not a callback framework.  A source can be made by combining a
   DAQ and a buffer. 

   This is part of shared.h as there will be references from Python as
   well.

*/

class cosink {
 public:
  virtual void connect_sink(boost::shared_ptr<sink>) = 0;
};

/* There is no cosource - the thing that reads a source.
   That would be your Python code. */





/* Wrapped constructors */

// 0-arg constructors are all the same pattern so use macro
#define wrap(cls) \
  static inline boost::shared_ptr<cls> shared_##cls() { \
    return boost::shared_ptr<cls>(new cls());           \
  }
wrap(uart)
wrap(syncser)
wrap(memory)
wrap(diff)
wrap(hole)
wrap(stack_program)
wrap(chunk_stack)


// multi-arg constructors are written out
static inline boost::shared_ptr<stack_op_sink>
shared_stack_op_sink(boost::shared_ptr<stack_op> program,
                     boost::shared_ptr<chunk_stack> stack) {
  return boost::shared_ptr<stack_op_sink>(new stack_op_sink(program, stack));
}

static inline boost::shared_ptr<memmap>
shared_memmap(const char *filename, uint64_t size) {
  return boost::shared_ptr<memmap>(new memmap(filename, size));
}

static inline boost::shared_ptr<compose_snk_op>
shared_compose_snk_op(boost::shared_ptr<sink> snk,
                      boost::shared_ptr<operation> op) {
  return boost::shared_ptr<compose_snk_op>(new compose_snk_op(snk, op));
}
static inline boost::shared_ptr<compose_op_op>
shared_compose_op_op(boost::shared_ptr<operation> op1,
                     boost::shared_ptr<operation> op2) {
  return boost::shared_ptr<compose_op_op>(new compose_op_op(op1, op2));
}
static inline boost::shared_ptr<compose_op_src>
shared_compose_op_src(boost::shared_ptr<operation> op,
                      boost::shared_ptr<source> src) {
  return boost::shared_ptr<compose_op_src>(new compose_op_src(op, src));
}



#endif
