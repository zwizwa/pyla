/* frontend.h - bitstream to bytes analyzer interface. 

   A frontend is an analyzer operating on the raw bit stream,
   returning a stream of bytes.

   FIXME: Bitstream is currently fixed at 8 bit.
*/

#ifndef _FRONTEND_H
#define _FRONTEND_H

#include <vector>

class frontend {
public:
  /* Parse a chunk of 8 channel binary data at samplerate, return a
     chunk of parsed bytes (representation not specified). */
  virtual std::vector<unsigned char> analyze(std::vector<unsigned char>) = 0;
  virtual void set_samplerate(double sr) = 0;
  /* Reset parser state machine. */
  virtual void reset() = 0;
};

#endif // _FRONTEND_H
