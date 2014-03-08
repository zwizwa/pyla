#ifndef _MIPMAP_H
#define _MIPMAP_H

#include <stdint.h>

typedef uint32_t word;

class mipmap : public sink {
 public:
  void write(chunk&);
  /* This encoding allows combining by logic OR. */
  static const low     = 1;
  static const high    = 2;
  static const both    = 3;
  static const neither = 0;

 private:
  _build(int);
  int _size;
  vector< vector<word> > _store;  // access store in native wordsize
};


#endif // _MIPMAP_H
