/*
  SESSION-SERVICE-EXPLORE-SCOPE.hpp  -  session implementation service API: explore scope

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


/** @file session-service-explore-scope.hpp
 ** Implementation level session API: query a scope.
 ** This specialised service is intended to be used by the Scope and
 ** QueryFocus framework for enumerating objects contained within a
 ** given scope and for locating the scope's parent scope. Basically,
 ** this service just exposes a QueryResolver, which is actually
 ** backed by the PlacementIndex and is able to handle queries of
 ** type ScopeQuery, especially ContentsQuery and PathQuery.
 ** 
 ** By virtue of this service, QueryFocus, Scope and Placement can
 ** remain completely agnostic of session's implementation details,
 ** especially they aren't bound to PlacementIndex. This is important,
 ** because the public session API is casted in terms of PlacementRef
 ** and QueryFocus. An implementation of this service is available
 ** through the SessionServices access mechanism.
 ** 
 ** @see session-impl.hpp implementation of the service
 ** @see session-services.cpp implementation of access
 **
 */


#ifndef MOBJECT_SESSION_SESSION_SERVICE_EXPLORE_SCOPE_H
#define MOBJECT_SESSION_SESSION_SERVICE_EXPLORE_SCOPE_H

#include "steam/mobject/placement.hpp"
#include "common/query/query-resolver.hpp"




namespace proc {
namespace mobject {
namespace session {
  
  
  
  /**
   * Implementation-level service for issuing contents/discovery queries.
   * Actually, the implementation of this service is backed by the PlacementIndex
   * within the current session, but this link isn't disclosed to client code.
   * The exposed QueryResolver is able to handle typed DiscoveryQuery instances.
   * Usually, on invocation, a search scope needs to be specified. The root Scope
   * of the current model (session datastructure) can be obtained by #getScopeRoot
   */
  struct SessionServiceExploreScope
    {
      static lumiera::QueryResolver const& getResolver();
      
      static PlacementMO& getScope (PlacementMO const&);
      static PlacementMO& getScope (PlacementMO::ID const&);
      static PlacementMO& getScopeRoot();
    };
  
  
  
}}} // namespace proc::mobject::session
#endif
