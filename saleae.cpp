/* Dump Saleae Logic output to stdout.
   Adapted from SaleaeDeviceSdk-1.1.14/source/ConsoleDemo.cpp */

#include "saleae.h"

#include <memory>
#include <iostream>
#include <string>


/* Put these as global static members in the C++ module, not as static
   memers of the class.  swig chokes on the __stdcall */

static void __stdcall OnConnect( U64 device_id, GenericInterface* device_interface, void* user_data );
static void __stdcall OnDisconnect( U64 device_id, void* user_data );
static void __stdcall OnReadData( U64 device_id, U8* data, U32 data_length, void* user_data );
static void __stdcall OnWriteData( U64 device_id, U8* data, U32 data_length, void* user_data );
static void __stdcall OnError( U64 device_id, void* user_data );
static saleae *_find(U64);
static void _start();



/* Registry */
static std::vector<saleae*> _device_map;
static mutex *_device_map_mutex;

static void _start() {
  static int global_init;
  if (!_device_map_mutex) {
    _device_map_mutex = new mutex();
    LOG("_start()\n");
    DevicesManagerInterface::RegisterOnConnect( &OnConnect );
    DevicesManagerInterface::RegisterOnDisconnect( &OnDisconnect );
    DevicesManagerInterface::BeginConnect();
    global_init = 1;
  }
}

static saleae *_register(U64 device_id, GenericInterface* device_interface) {
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
static saleae *_find(U64 device_id) {
  return _register(device_id, NULL);
}



void saleae::disconnect_sinks() {
  LOG("disconnect_sinks()\n");
  _device_map_mutex->lock();
  int i, n = _device_map.size();
  for(i=0; i<n; i++) {
    _device_map[i]->connect_sink(NULL);
  }
  _device_map_mutex->unlock();
}

std::vector<saleae*> saleae::devices() {
  _start();
  _device_map_mutex->lock();
  std::vector<saleae*> _map_copy = _device_map;
  _device_map_mutex->unlock();
  return _map_copy;
}


void __stdcall OnReadData( U64 device_id, U8* data, U32 data_length, void* user_data ) {
  _find(device_id)->on_read(data, data_length);
  // We own, so need to delete.
  DevicesManagerInterface::DeleteU8ArrayPtr( data );
}
void __stdcall OnWriteData( U64 device_id, U8* data, U32 data_length, void* user_data ) {
    /* Not used */
}
void __stdcall OnError( U64 device_id, void* user_data ) {
  LOG("ERROR\n");
  _find(device_id)->on_error();
}
void __stdcall OnDisconnect( U64 device_id, void* user_data ) {
  _find(device_id)->on_disconnect();
}
void __stdcall OnConnect( U64 device_id, GenericInterface* device_interface, void* user_data ) {
  if( dynamic_cast<LogicInterface*>( device_interface ) != NULL ) {
    LOG("Connect %08x\n", device_id);
    LogicInterface *i = (LogicInterface*)device_interface;

    saleae *dev = _register(device_id, i);

    i->RegisterOnReadData( &OnReadData );
    i->RegisterOnWriteData( &OnWriteData );
    i->RegisterOnError( &OnError );
    i->SetSampleRateHz( dev->get_samplerate() );
    // Start automatically
    i->ReadStart();
  }
}




saleae::saleae(U64 device_id, GenericInterface* device_interface) :
  _device_id(device_id),
  _device_interface(device_interface),
  _samplerate(4000000),
  _sink(NULL)
{
  _start();
}
saleae::~saleae() {
  // Tear down the callback before deleting any instances.
  LOG("~saleae()\n");
}

double saleae::get_samplerate() {
  return _samplerate;
}
void saleae::set_samplerate_hint(double sr) {
  _samplerate = sr;
}
void saleae::connect_sink(sink *s) {
  LOG("<%p>->connect_sink(<%p>)\n", this, s);
  _sink_mutex.lock();
  _sink = s;
  _sink_mutex.unlock();
}

void saleae::on_read(U8* data, U32 data_length) {
  _sink_mutex.lock();
  if (_sink) {
    chunk input;
    input.assign(data, data + data_length);
    _sink->write(input);
  }
  _sink_mutex.unlock();
}
void saleae::on_error() {
  // FIXME
  LOG("on_error\n");
}
void saleae::on_disconnect() {
  // FIXME
  LOG("on_error\n");
}

U64 saleae::get_device_id() {
  return _device_id; 
}



