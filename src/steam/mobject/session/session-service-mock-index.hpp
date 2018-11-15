/*
  SESSION-SERVICE-MOCK-INDEX.hpp  -  session service API: mock PlacementIndex for tests

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


/** @file session-service-mock-index.hpp
 ** Implementation level session API: PlacementIndex mock for tests.
 ** Allows (temporarily) to replace the real Placement index within
 ** the session by a mock instance installed and provided through
 ** this API. Unit tests may use this \em backdoor to set up a
 ** specially prepared index to verify the behaviour of Placement
 ** and Scope resolution operations.
 ** 
 ** The test/mock instance of the placement index obtained by this API
 ** is \em not wired with the session. Rather it is managed by smart-ptr.
 ** When the last smart-ptr instance goes out of scope, the test index
 ** instance will be shut down and removed, thereby uncovering the 
 ** original PlacementIndex living within the session.
 ** 
 ** @see session-impl.hpp implementation of the service
 ** @see session-services.cpp implementation of access
 **
 */


#ifndef MOBJECT_SESSION_SESSION_SERVICE_MOCK_INDEX_H
#define MOBJECT_SESSION_SESSION_SERVICE_MOCK_INDEX_H

#include "steam/mobject/session/placement-index.hpp"

#include <memory>




namespace proc {
namespace mobject {
namespace session {
  
  typedef std::shared_ptr<PlacementIndex> PPIdx;
  

  /** there is an implicit PlacementIndex available on a global level,
   *  by default implemented within the current session. This Service
   *  temporarily overlays a newly created mock instance, e.g. for tests.
   *  @return smart-ptr managing a newly created mock index instance.
   *         Any implicit access to the session's placement index will
   *         be redirected to that instance. When the smart-ptr reaches
   *         use-count zero, access to the original PlacementIndex
   *         will be restored.
   */
  class SessionServiceMockIndex
    {
    public:
      static PPIdx install ();
    };
  
  
  
}}} // namespace proc::mobject::session
#endif
