#ifndef _SALEAE_H
#define _SALEAE_H

#include "pylacore.h"
#include <SaleaeDeviceApi.h>
#include <vector>


class saleae : public sampler {
 public:
  saleae(U64 device_id, GenericInterface* device_interface);
  ~saleae();
  double get_samplerate();
  void read(chunk&);
  void set_samplerate_hint(double sr);

  void on_read(U8* data, U32 data_length);
  void on_error();
  void on_disconnect();
  U64 get_device_id();

  static std::vector<saleae*> devices();

 private:
  U64 _device_id;
  GenericInterface* _device_interface;
  double _samplerate;
  memory _buffer;
};

#endif // _SALEAE_H


