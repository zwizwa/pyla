#ifndef _SALEAE_H
#define _SALEAE_H

#include "pylacore.h"
#include <SaleaeDeviceApi.h>
#include <vector>


class saleae : public cosink, public sampler {
 public:
  /* cosink */
  void connect_sink(sink*);
  
  /* sampler */
  double get_samplerate();
  void set_samplerate_hint(double sr);

  /* specific */
  saleae(U64 device_id, GenericInterface* device_interface);
  ~saleae();
  void on_read(U8* data, U32 data_length);
  void on_error();
  void on_disconnect();
  U64 get_device_id();

  static std::vector<saleae*> devices();
  static void disconnect_sinks();

 private:
  U64 _device_id;
  GenericInterface* _device_interface;
  double _samplerate;
  sink* _sink;
  mutex _sink_mutex;
};

#endif // _SALEAE_H


