/* Shared object wrapping.  These are separate to keep the leaf
   classes clean for reuse. */

#ifndef _SHARED_H
#define _SHARED_H

#include "pylacore.h"
#include "uart.h"
#include "syncser.h"
#include "measure.h"
#include "rpn.h"


/* Wrapped constructors */

// 0-arg constructors are all the same pattern so use macro
#define pyla_wrap(cls) \
  static inline boost::shared_ptr<cls> shared_##cls() { \
    return boost::shared_ptr<cls>(new cls());           \
  }
pyla_wrap(uart)
pyla_wrap(syncser)
pyla_wrap(memory)
pyla_wrap(dedup)
pyla_wrap(hole)
pyla_wrap(stack_program)
pyla_wrap(chunk_stack)
pyla_wrap(frequency)

// multi-arg constructors are written out
static inline boost::shared_ptr<stack_op_sink>
shared_stack_op_sink(boost::shared_ptr<stack_op> program) {
  return boost::shared_ptr<stack_op_sink>(new stack_op_sink(program));
}
static inline boost::shared_ptr<memmap>
shared_memmap(const char *filename, uint64_t size) {
  return boost::shared_ptr<memmap>(new memmap(filename, size));
}



#endif
