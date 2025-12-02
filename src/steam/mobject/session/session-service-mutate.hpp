/*
  SESSION-SERVICE-MUTATE.hpp  -  session implementation service API: add/remove session contents

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file session-service-mutate.hpp
 ** Implementation level session API: add or remove Session contents.
 ** This specialised service is intended to be used by MObjectRef,
 ** in order to attach a new Placement to the session or to detach
 ** and purge an existing Placement. An implementation of this service
 ** service is available through the SessionServices access mechanism,
 ** and delegates the actual implementation to the PlacementIndex,
 ** which is the core session datastructure.
 ** 
 ** @see session-impl.hpp implementation of the service
 ** @see session-services.cpp implementation of access
 **
 */


#ifndef MOBJECT_SESSION_SESSION_SERVICE_MUTATE_H
#define MOBJECT_SESSION_SESSION_SERVICE_MUTATE_H

//#include "steam/mobject/session.hpp"
//#include "lib/meta/generator.hpp"
#include "steam/mobject/placement.hpp"




namespace steam {
namespace mobject {
namespace session {
  
  
  /**
   * Implementation-level service for resolving an Placement-ID.
   * Usually, this service is backed by the PlacementIndex of the
   * current session -- however, for the purpose of unit testing,
   * this index may be overlaid temporarily, by using the
   * SessionServiceMockIndex API.
   */
  class SessionServiceMutate
    {
    public:
      typedef PlacementMO     const& PMO;
      typedef PlacementMO::ID const& PID;
      
      static PID  attach_toModel (PMO, PID) ;
      static bool detach_and_clear (PID) ;
      static bool detach (PID) ;
    };
  
  
  
}}} // namespace steam::mobject::session
#endif
