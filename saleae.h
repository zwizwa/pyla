#ifndef _SALEAE_H
#define _SALEAE_H

#include "pylacore.h"
#include <SaleaeDeviceApi.h>
#include <vector>
#include <mutex.h>


class saleae : public sampler {
 public:
  saleae(U64 device_id, GenericInterface* device_interface);
  ~saleae();
  double get_samplerate();
  std::vector<unsigned char> read();
  void read_sync();
  void set_samplerate_hint(double sr);
  void set_buffer(buffer *);

  void on_read(U8* data, U32 data_length);
  void on_error();
  void on_disconnect();
  U64 get_device_id();

  static std::vector<saleae*> devices();

 private:
  mutex _mutex;
  U64 _device_id;
  GenericInterface* _device_interface;
  double _samplerate;
  buffer *_buffer;
};

#endif // _SALEAE_H


