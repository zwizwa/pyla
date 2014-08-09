/* Dump Saleae Logic output to stdout.
   Adapted from SaleaeDeviceSdk-1.1.14/source/ConsoleDemo.cpp */

#include "sigrok.h"

#include <memory>
#include <iostream>
#include <string>
#include <libsigrok/libsigrok.h>

using boost::shared_ptr;
using std::cout;
using std::cerr;
using std::endl;

static void wrap_datafeed_in(const struct sr_dev_inst *sdi,
                             const struct sr_datafeed_packet *packet,
                             void *cb_data) {
  sigrok *s = (sigrok *)cb_data;
  cerr << ".";
  //s->datafeed_in(sdi, packet);
}

sigrok::sigrok() :
  _inst(NULL),
  _samplerate(PYLA_DEFAULT_SAMPLERATE),
  _sink(shared_ptr<sink>(new hole())) {
  int rv;
  if (SR_OK != (rv = sr_init(&_sr))) {
    g_critical("sr_init() failed");
    exit(1); // FIXME
  }
  cout << "sigrok.cpp: scanning drivers" << endl;
  struct sr_dev_driver **drivers = sr_driver_list();
  for (int i = 0; drivers[i]; i++) {
    if(strcmp(drivers[i]->name, "fx2lafw")) continue; // FIXME: later use other
    cout << " - " << drivers[i]->name << ": " << drivers[i]->longname << endl;
    if (sr_driver_init(_sr, drivers[i]) != SR_OK) {
      g_critical("Failed to initialize driver.");
      exit(1); // FIXME
    }
    GSList *tmpdevs = sr_driver_scan(drivers[i], NULL);
    for (GSList *l = tmpdevs; l; l = l->next) {
      struct sr_dev_inst *inst = (typeof(inst))l->data;
      int nchan = g_slist_length(inst->channels);
      cout << "   - " << inst << ": " << nchan << endl;
      if (SR_OK != sr_dev_open(inst)) {
        g_critical("Failed to open device");
        exit(1);
      }
      _inst = inst;
      sr_session_new(&_session);
      sr_session_datafeed_callback_add(_session, wrap_datafeed_in, this);
      if (sr_session_dev_add(_session, _inst) != SR_OK) {
        g_critical("Failed to add device to session.");
        sr_session_destroy(_session);
        exit(1); 
      }
      for (GSList *l = _inst->channels; l; l = l->next) {
        struct sr_channel *ch = (typeof(ch))l->data;
        ch->enabled = TRUE; // enable all, we do our own filtering & triggering
      }

    }
    g_slist_free(tmpdevs);
  }
}

void sigrok::start() {
  if (sr_session_start(_session) != SR_OK) {
    g_critical("Failed to start session.");
    sr_session_destroy(_session);
    exit(1);
  }
  sr_session_run(_session);
}

sigrok::~sigrok() {
  // Tear down the callback before deleting any instances.
  LOG("sigrok.cpp:~sigrok()\n");
  
}


double sigrok::get_samplerate() {
  return _samplerate;
}
void sigrok::set_samplerate_hint(double sr) {
  _samplerate = sr;
}
void sigrok::connect_sink(shared_ptr<sink> s) {
  LOG("sigrok.cpp:connect_sink\n");
  // _sink_mutex.lock();  // FIXME!
  _sink = s;
  // _sink_mutex.unlock();
}



