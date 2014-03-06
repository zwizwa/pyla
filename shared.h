#ifndef _FACTORY_H
#define _FACTORY_H

#include "pylacore.h"
#include "uart.h"
#include "syncser.h"

#include <boost/shared_ptr.hpp>




/* Shared object factory. 
   Let's keep these separate to keep the classes clean for reuse. */



// multi-arg constructors are written out
static inline boost::shared_ptr<file>
shared_file(const char *filename, uint64_t size) {
  return boost::shared_ptr<file>(new file(filename, size));
}

// 0-arg constructors are all the same pattern so use macro
#define wrap(cls) \
  static inline boost::shared_ptr<cls> shared_##cls() { \
    return boost::shared_ptr<cls>(new cls());           \
  }
wrap(uart)
wrap(syncser)
wrap(memory)
wrap(diff)
wrap(blackhole)


/* Compositions use shared pointers to enforce the use of shared
   factory methods instead of raw constructors from python.  For each
   composition constructor, also provide a shared_ptr wrapped version.
 */
class compose_snk_op : public sink {
 public:
  compose_snk_op(boost::shared_ptr<sink> snk,
                 boost::shared_ptr<operation> op)
    : _op(op), _snk(snk) { }
  ~compose_snk_op() {  LOG("~compose_snk_op()\n"); }
  void write(chunk& input) {
    chunk output;
    _op->process(output, input);
    if (!output.empty()) {
      _snk->write(output);
    }
  }
 private:
  boost::shared_ptr<operation> _op;
  boost::shared_ptr<sink> _snk;
};
static inline boost::shared_ptr<compose_snk_op>
shared_compose_snk_op(boost::shared_ptr<sink> snk,
                      boost::shared_ptr<operation> op) {
  return boost::shared_ptr<compose_snk_op>(new compose_snk_op(snk, op));
}



#endif
