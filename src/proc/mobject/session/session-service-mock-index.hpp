/*
  SESSION-SERVICE-MOCK-INDEX.hpp  -  session service API: mock PlacementIndex for tests
 
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


/** @file session-service-mock-index.hpp
 ** Implementation level session API: PlacementIndex mock for tests.
 ** Allows (temporarily) to replace the real Placement index within
 ** the session by a mock instance handed in through this API. Unit
 ** tests may use this \em backdoor to set up a specially prepared 
 ** index to verify the behaviour of Placement and Scope resolution
 ** operations.
 ** 
 ** @see session-impl.hpp implementation of the service
 ** @see session-services.cpp implementation of access
 **
 */


#ifndef MOBJECT_SESSION_SESSION_SERVICE_MOCK_INDEX_H
#define MOBJECT_SESSION_SESSION_SERVICE_MOCK_INDEX_H

//#include "proc/mobject/session.hpp"
//#include "lib/meta/generator.hpp"




namespace mobject {
namespace session {
  
//  using lumiera::typelist::InstantiateChained;
//  using lumiera::typelist::InheritFrom;
//  using lumiera::typelist::NullType;
  
  
  class SessionServiceMockIndex
    {
    };
  
  
  
}} // namespace mobject::session
#endif
