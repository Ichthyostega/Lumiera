/*
  SESSION-SERVICE-EXPLORE-SCOPE.hpp  -  session implementation service API: explore scope
 
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
 ** and especially aren't bound to PlacementIndex. This is important,
 ** because the public session API is casted in terms of PlacementRef
 ** and QueryFocus An implementation of this service is available
 ** through the SessionServices access mechanism.
 ** 
 ** @see session-impl.hpp implementation of the service
 ** @see session-services.cpp implementation of access
 **
 */


#ifndef MOBJECT_SESSION_SESSION_SERVICE_EXPLORE_SCOPE_H
#define MOBJECT_SESSION_SESSION_SERVICE_EXPLORE_SCOPE_H

#include "proc/mobject/session/query-resolver.hpp"
//#include "lib/meta/generator.hpp"




namespace mobject {
namespace session {
  
//  using lumiera::typelist::InstantiateChained;
//  using lumiera::typelist::InheritFrom;
//  using lumiera::typelist::NullType;
  
  
  struct SessionServiceExploreScope
    {
      static QueryResolver& getResolver();
    };
  
  
  
}} // namespace mobject::session
#endif