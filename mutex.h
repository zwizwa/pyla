#ifndef _MUTEX_H
#define _MUTEX_H
// FIXME: later replace with boost?

#if defined(_WIN32)
// FIXME: no locking on windows!
class mutex {
 public:
  mutex()       {}
  ~mutex()      {}
  void lock()   {}
  void unlock() {}
};
#else
#include <pthread.h>
class mutex {
 public:
  mutex()       {pthread_mutex_init(&_mutex, NULL);}
  ~mutex()      {}
  void lock()   {pthread_mutex_lock(&_mutex);}
  void unlock() {pthread_mutex_unlock(&_mutex);}
 private:
  pthread_mutex_t _mutex;
};
#endif

#endif
