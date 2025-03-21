/*
  SESSION-SERVICE-FETCH.hpp  -  session implementation service API: fetch PlacementRef

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file session-service-fetch.hpp
 ** Implementation level session API: resolve a Placement by hash-ID.
 ** This specialised service is intended to be used by PlacementRef,
 ** in order to (re)-access the Placement instance within the session,
 ** given the hash-ID of this placement. An implementation of this
 ** service is available through the SessionServices access mechanism.
 ** 
 ** @see session-impl.hpp implementation of the service
 ** @see session-services.cpp implementation of access
 **
 */


#ifndef MOBJECT_SESSION_SESSION_SERVICE_FETCH_H
#define MOBJECT_SESSION_SESSION_SERVICE_FETCH_H

//#include "steam/mobject/session.hpp"
//#include "lib/meta/generator.hpp"
#include "steam/mobject/placement.hpp"




namespace steam {
namespace mobject {
namespace session {
  
//  using lib::meta::InstantiateChained;
//  using lib::meta::InheritFrom;
//  using lib::meta::NullType;
  
  /**
   * Implementation-level service for resolving an Placement-ID.
   * Usually, this service is backed by the PlacementIndex of the
   * current session -- however, for the purpose of unit testing,
   * this index may be overlaid temporarily, by using the
   * SessionServiceMockIndex API.
   */
  class SessionServiceFetch
    {
    public:
      static PlacementMO& resolveID (PlacementMO::ID const&) ;
      static bool    isRegisteredID (PlacementMO::ID const&) ;
      
      static bool isAccessible() ;
    };
  
  
  
}}} // namespace steam::mobject::session
#endif
