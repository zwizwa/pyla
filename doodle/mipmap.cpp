#include "mipmap.h"

static inline uint64_t log2_upper(uint64_t v) {
  v--;
  uint64_t log = 0;
  while(v) {
    log++;
    v>>=1;
  }
  return log;
}

/* Store increments in powers of two.
   Mipmap is stored as an array of arrays.
   Unused space is filled with 'neither'. */

/* Precond: level sizes are correct. */
static inline _build_single(word *fine, word *coarse, uint64_t coarse_length) {
  for (int i = 0; i < coarse_length; i++) {
    coarse[i] = fine[i * 2] | fine[i * 2 + 1];
  }
}

/* Using a flat power-of-two buffer makes data access very simple.
   The pattern in the address bits can be used to access levels
   directly using a signed int shift trick.

0.....
10....
110...
1110..
11110.
*/

static inline level_address(uint64_t index, uint64_t level, uint64_t log2size) {
  uint64_t mask    = (1<<log2size)-1;
  int64_t  onebits = (-1)&(~mask);
  int64_t  addr    = ((onebits << level) | (index > level)) & mask;
  return addr;
}

/* first level -> compute from raw data. */

void mipmap::_build(word *store, uint64_t size) {
  /* Build first level from store. */

  /* Recursively build other levels. */
  uint64_t levels = log2_upper(size);
  for (uint64_t l = 1; l < level; l++) {
    word *fine   = word + level_address(0, level-1, levels);
    word *coarse = word + level_address(0, level,   levels);
    _build_single(fine, coarse);
  }
}

/*

  TODO:
  - use disk-backed memory-mapped buffer
  - fix number of channels
  - fix maximum buffer size

  -> this makes on-the fly updates really simple + leaves memory
     management to the OS.

 */
