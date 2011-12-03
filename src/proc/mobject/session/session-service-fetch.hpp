/*
  SESSION-SERVICE-FETCH.hpp  -  session implementation service API: fetch PlacementRef

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

//#include "proc/mobject/session.hpp"
//#include "lib/meta/generator.hpp"
#include "proc/mobject/placement.hpp"




namespace proc {
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
  
  
  
}}} // namespace proc::mobject::session
#endif
