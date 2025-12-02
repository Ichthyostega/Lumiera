/*
  DummyPlayConnection.hpp  -  simplified test setup for playback

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file dummy-play-connection.cpp
 ** Implementation of a service for mock render playback to support testing.
 */


#include "steam/play/dummy-play-connection.hpp"
//#include "steam/play/dummy-image-generator.hpp"
//#include "steam/play/tick-service.hpp"
//#include "lib/depend.hpp"

//#include <string>
//#include <memory>
//#include <functional>



namespace steam{
namespace play {
namespace test {
  
  
  namespace { // hidden local details of the service implementation....
    
    
  } // (End) hidden service impl details
  
  UnimplementedConnection::~UnimplementedConnection() { } ///< emit VTable here...
  
  
  
  /** */
  
  
}}} // namespace steam::play
