
#include "pylacore.h"
#include "uart.h"
#include "syncser.h"
#include "saleae.h"
#include "rpn.h"
#include "shared.h"
#include "measure.h"

#include <boost/python.hpp>
using namespace boost::python;

struct factory {
  static boost::shared_ptr<std::vector<unsigned char> > buffer(unsigned int n) {
    return boost::shared_ptr<std::vector<unsigned char> >
      ( new std::vector<unsigned char>(n) );
  }
};

/* FIXME: missing some basic ideas about memory management and data
   conversion -> read manual!

   Swig did automatic data conversion.  Boost Python doesn't seem to
   do that for vectors.

   Most of this can be avoided by never exposing the data types to
   python, but keeping them hidden behind a "patcher" API.  Results
   can then be copied to python values explicitly.

*/


BOOST_PYTHON_MODULE(pylacore)
{
  // Pure virtual classes probably don't need to be exposed?
  // class_<operation>("operation")
  //   .def("process",        pure_virtual(&operation::process))
  //   ;
  // class_<frontend, bases<operation> >("frontend")
  //   .def("set_samplerate", pure_virtual(&frontend::set_samplerate))
  //   .def("reset",          pure_virtual(&frontend::reset))
  //   ;
  //class_<uart, bases<frontend> >("uart")

  class_<uart>("uart")
    .def("set_baudrate",   &uart::set_baudrate)
    .def("set_channel",    &uart::set_channel)
    .def("process",        &uart::process)
    .def("set_samplerate", &uart::set_samplerate)
    .def("reset",          &uart::reset)
    ;

  // https://wiki.python.org/moin/boost.python/PointersAndSmartPointers
  class_<factory, boost::shared_ptr<factory> >("factory",init<>())
    .def("buffer",&factory::buffer).staticmethod("buffer")
    ;
}
