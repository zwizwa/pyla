#ifndef _SALEAE_H
#define _SALEAE_H

#include "shared.h"
#include <SaleaeDeviceApi.h>
#include <vector>


class saleae : public cosink, public sampler {
 public:
  /* cosink */
  void connect_sink(boost::shared_ptr<sink>);
  
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

 private:
  U64 _device_id;
  GenericInterface* _device_interface;
  double _samplerate;
  boost::shared_ptr<sink> _sink;
  mutex _sink_mutex;
};

#endif // _SALEAE_H


