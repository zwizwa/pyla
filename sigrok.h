#ifndef _SIGROK_H
#define _SIGROK_H

#include "shared.h"
#include <vector>


class sigrok : public cosink, public sampler {
 public:
  sigrok();
  ~sigrok();
  /* cosink */
  void connect_sink(boost::shared_ptr<sink>);
  
  /* sampler */
  double get_samplerate();
  void set_samplerate_hint(double sr);

  void start();

 private:
  boost::shared_ptr<sink> _sink;
  double _samplerate;
  // FIXME: shared context?
  struct sr_context *_sr;
  struct sr_dev_inst *_inst;
  struct sr_session *_session;

};

#endif // _SIGROK_H


