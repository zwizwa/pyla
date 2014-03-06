#ifndef _FACTORY_H
#define _FACTORY_H

#include "pylacore.h"
#include "uart.h"
#include "syncser.h"

#include <boost/shared_ptr.hpp>


/* Shared object factory. 
   Let's keep these separate to keep the classes clean. */

boost::shared_ptr<uart>    make_uart();
boost::shared_ptr<syncser> make_syncser();


#endif
