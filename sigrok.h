#ifndef _SIGROK_H
#define _SIGROK_H

#include "shared.h"
#include <vector>
#include <boost/thread.hpp>


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
  void thread_main();

  void datafeed_in(const struct sr_dev_inst *sdi,
                   const struct sr_datafeed_packet *packet);

 private:
  boost::shared_ptr<sink> _sink;
  boost::mutex _sink_mutex;
  boost::thread *_thread;
  double _samplerate;
  // FIXME: shared context?
  struct sr_context *_sr;
  struct sr_dev_inst *_inst;
  struct sr_session *_session;

};

#endif // _SIGROK_H


