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
  ~saleae();
  void on_read(U8* data, U32 data_length);
  void on_error();
  void on_disconnect();
  U64 get_device_id();

  static std::vector<saleae*> devices();
  static saleae *register_device(U64 device_id, GenericInterface* device_interface);
  static saleae *find_device(U64 device_id);
  static void start(double sample_rate);

 private:
  saleae(U64 device_id, GenericInterface* device_interface);
  static void _start();

  U64 _device_id;
  GenericInterface* _device_interface;
  double _samplerate;
  boost::shared_ptr<sink> _sink;
  boost::mutex _sink_mutex;

  /* Registry owns saleae instances
     Never delete a saleae object, nor remove it from this list. */
  static std::vector<saleae*> _device_map;
  static boost::mutex *_device_map_mutex;
  static double _default_samplerate;
};

#endif // _SALEAE_H


