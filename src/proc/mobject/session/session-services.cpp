/*
  SessionServices  -  accessing Proc-Layer internal session implementation services
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "proc/mobject/session/session-service-fetch.hpp"
#include "proc/mobject/session/session-service-explore-scope.hpp"
#include "proc/mobject/session/session-service-mock-index.hpp"
#include "proc/mobject/session/session-service-defaults.hpp"

#include "proc/mobject/session/session-services.hpp"
#include "proc/mobject/session/session-impl.hpp"
#include "proc/mobject/session/sess-manager-impl.hpp"

namespace mobject {
namespace session {
  
  /** verify the given placement-ID (hash) is valid,
   *  by checking if it refers to a Placement instance
   *  currently registered with the PlacementIndex of the
   *  active Session. */
  bool
  SessionServiceFetch::isRegisteredID (PlacementMO::ID const& placementID)
  {
    return SessionImplAPI::current->isRegisteredID (placementID);
  }
  
  
  /** actually retrieve a Placement tracked by the index.
   *  @param placementID hash-ID, typically from a PlacementRef
   *  @throw error::Invalid if the ID isn't resolvable
   *  @note the returned ref is guaranteed to be valid and usable
   *        only \em now, which means, by virtue of the ProcDispatcher
   *        and command processing, during this operation. It can be
   *        used to invoke an operation, but should never be stored;
   *        rather, client code should create an MObjectRef, if 
   *        bound to store an reference for later.
   */
  PlacementMO&
  SessionServiceFetch::resolveID (PlacementMO::ID const& placementID)
  {
    return SessionImplAPI::current->resolveID (placementID);
  }
  
  
  /** */
  void
  SessionServiceMockIndex::reset_PlacementIndex (PPIdx const& alternativeIndex)
  {
    return SessionImplAPI::current->reset_PlacementIndex (alternativeIndex);
  }

  
  
  
  
}} // namespace mobject::session
