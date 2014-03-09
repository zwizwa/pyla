/* Dump Saleae Logic output to stdout.
   Adapted from SaleaeDeviceSdk-1.1.14/source/ConsoleDemo.cpp */

#include "saleae.h"

#include <memory>
#include <iostream>
#include <string>

using boost::shared_ptr;


/* Put these as global static members in the C++ module, not as static
   memers of the class.  swig chokes on the __stdcall */

static void __stdcall OnConnect( U64 device_id, GenericInterface* device_interface, void* user_data );
static void __stdcall OnDisconnect( U64 device_id, void* user_data );
static void __stdcall OnReadData( U64 device_id, U8* data, U32 data_length, void* user_data );
static void __stdcall OnWriteData( U64 device_id, U8* data, U32 data_length, void* user_data );
static void __stdcall OnError( U64 device_id, void* user_data );

void __stdcall OnReadData( U64 device_id, U8* data, U32 data_length, void* user_data ) {
  saleae::find_device(device_id)->on_read(data, data_length);
  // We own, so need to delete.
  DevicesManagerInterface::DeleteU8ArrayPtr( data );
}
void __stdcall OnWriteData( U64 device_id, U8* data, U32 data_length, void* user_data ) {
    /* Not used */
}
void __stdcall OnError( U64 device_id, void* user_data ) {
  LOG("salea.cpp:ERROR\n");
  saleae::find_device(device_id)->on_error();
}
void __stdcall OnDisconnect( U64 device_id, void* user_data ) {
  saleae::find_device(device_id)->on_disconnect();
}
void __stdcall OnConnect( U64 device_id, GenericInterface* device_interface, void* user_data ) {
  if( dynamic_cast<LogicInterface*>( device_interface ) != NULL ) {
    LOG("salea.cpp:%08X Connect\n", device_id);
    LogicInterface *i = (LogicInterface*)device_interface;
    saleae *dev = saleae::register_device(device_id, i);
    i->RegisterOnReadData( &OnReadData );
    i->RegisterOnWriteData( &OnWriteData );
    i->RegisterOnError( &OnError );
    double sr = dev->get_samplerate();
    i->SetSampleRateHz( sr );
    LOG("salea.cpp:%08X Start at %.3f MHz\n", device_id, sr/1000000);
    i->ReadStart();
  }
}



/* saleae class */
std::vector<saleae*> saleae::_device_map;
mutex* saleae::_device_map_mutex;
double saleae::_default_samplerate;

saleae::saleae(U64 device_id, GenericInterface* device_interface) :
  _device_id(device_id),
  _device_interface(device_interface),
  _samplerate(_default_samplerate),
  _sink(shared_ptr<sink>(new hole())) { }

saleae::~saleae() {
  // Tear down the callback before deleting any instances.
  LOG("salea.cpp:~saleae()\n");
}

void saleae::start(double samplerate) {
  if (!_device_map_mutex) {
    _default_samplerate = samplerate;
    _device_map_mutex = new mutex();
    DevicesManagerInterface::RegisterOnConnect( &OnConnect );
    DevicesManagerInterface::RegisterOnDisconnect( &OnDisconnect );
    DevicesManagerInterface::BeginConnect();
    LOG("salea.cpp:BeginConnect (waiting for Connect)\n");
  }
}

std::vector<saleae*> saleae::devices() {
  saleae::start(PYLA_DEFAULT_SAMPLERATE);
  _device_map_mutex->lock();
  std::vector<saleae*> _map_copy = _device_map;
  _device_map_mutex->unlock();
  return _map_copy;
}

saleae * saleae::register_device(U64 device_id,
                                 GenericInterface* device_interface) {
  _device_map_mutex->lock();
  /* It doesn't seem that we can use user_data to associate an instance
     to each device_id, so do dispatch based on a device_map */
  int i, n = _device_map.size();
  saleae *d = NULL;
  for(i=0; i<n; i++) {
    d = _device_map[i];
    if (d->get_device_id() == device_id) break;
  }
  if (!d) {
    if (device_interface) {
      d = new saleae(device_id, device_interface);
      _device_map.push_back(d);
    }
  }
  _device_map_mutex->unlock();
  return d;
}

saleae *saleae::find_device(U64 device_id) {
  return register_device(device_id, NULL);
}

double saleae::get_samplerate() {
  return _samplerate;
}
void saleae::set_samplerate_hint(double sr) {
  _samplerate = sr;
}
void saleae::connect_sink(shared_ptr<sink> s) {
  LOG("salea.cpp:%08X connect_sink\n", _device_id);
  _sink_mutex.lock();
  _sink = s;
  _sink_mutex.unlock();
}



static void dont_delete (chunk* p) {
  LOG("dont_delete(%p)\n", p);
}

void saleae::on_read(U8* data, U32 data_length) {
  _sink_mutex.lock();
  if (_sink) {
    boost::shared_ptr<chunk> c = 
      boost::shared_ptr<chunk>(new chunk(data_length), dont_delete);
    c->assign(data, data + data_length);
    _sink->write(c);
  }
  _sink_mutex.unlock();
}
void saleae::on_error() {
  // FIXME
  LOG("on_error\n");
}
void saleae::on_disconnect() {
  // FIXME
  LOG("on_disconnect\n");
}

U64 saleae::get_device_id() {
  return _device_id; 
}



