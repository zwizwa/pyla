/* pylacore.h - bitstream to bytes analyzer interfaces. 

   A frontend is an analyzer operating on the raw bit stream,
   returning a stream of bytes.

   FIXME: Bitstream is currently fixed at 8 bit.
*/

#ifndef _PYLACORE_H
#define _PYLACORE_H

#include <vector>

class analyzer {
public:
  /* KISS: byte vector to byte vector conversion. */
  virtual std::vector<unsigned char> analyze(std::vector<unsigned char>) = 0;
};

class frontend : public analyzer {
public:
  /* Parse a chunk of 8 channel binary data at samplerate, return a
     chunk of parsed bytes (representation not specified). */
  virtual std::vector<unsigned char> analyze(std::vector<unsigned char>) = 0;
  virtual void set_samplerate(double sr) = 0;
  /* Reset parser state machine. */
  virtual void reset() = 0;
};

#endif // _PYLACORE_H
