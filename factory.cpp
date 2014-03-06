#include "factory.h"

#define factory(cls) \
  boost::shared_ptr<cls> make_##cls() {         \
  boost::shared_ptr<cls> p(new cls());          \
  return p;                                     \
  }

factory(uart);
factory(syncser);
