
// For an example: look at sigrok-cli source code.

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
  s->datafeed_in(sdi, packet);
}

void sigrok::datafeed_in(const struct sr_dev_inst *sdi,
                         const struct sr_datafeed_packet *packet) {
  switch(packet->type) {
  case SR_DF_HEADER:
    cerr << "H";
    break;
  case SR_DF_END:
    cerr << "E";
    break;
  case SR_DF_LOGIC:
    {
      const struct sr_datafeed_logic *logic = (typeof(logic))packet->payload;
      const uint8_t *data = (typeof(data))logic->data;
      // FIXME: code only supports logic->unitsize == 1
      // cerr << logic->length;   // number of bytes
      // cerr << logic->unitsize; // size in bytes of a single unit
      // cerr << data[0];
      cerr << ".";
      _sink_mutex.lock();
      if (_sink) {
        boost::shared_ptr<chunk> c = 
          boost::shared_ptr<chunk>(new chunk(logic->length));
        c->assign(data, data + logic->length);
        _sink->write(c);
      }
      _sink_mutex.unlock();
    }
    break;
  default:
    cerr << "[type:" << packet->type << "]";
    break;
  }
}

sigrok::sigrok() :
  _session(NULL),
  _inst(NULL),
  _thread(NULL),
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
    // ->name ->longname
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
      break;
    }
    g_slist_free(tmpdevs);
  }
}

void sigrok::thread_main() {
  if (SR_OK != sr_config_set
      (_inst, NULL,
       SR_CONF_SAMPLERATE,
       g_variant_new_uint64(_samplerate))) {
    g_critical("Failed to configure samplerate.");
  }
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
  if (sr_session_start(_session) != SR_OK) {
    g_critical("Failed to start session.");
    sr_session_destroy(_session);
    exit(1);
  }
  sr_session_run(_session);
}
static void wrap_thread_main(sigrok *s) {
  s->thread_main();
}

void sigrok::start() {
  _thread = new boost::thread(wrap_thread_main, this);
}




sigrok::~sigrok() {
  // Tear down the callback before deleting any instances.
  LOG("sigrok.cpp: ~sigrok()\n");
  
}


double sigrok::get_samplerate() {
  return _samplerate;
}
void sigrok::set_samplerate_hint(double sr) {
  _samplerate = sr;
}
void sigrok::connect_sink(shared_ptr<sink> s) {
  LOG("sigrok.cpp: connect_sink\n");
  _sink_mutex.lock();
  _sink = s;
  _sink_mutex.unlock();
}



