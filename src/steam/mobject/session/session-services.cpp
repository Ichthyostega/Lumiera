/*
  SessionServices  -  accessing Steam-Layer internal session implementation services

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

* *****************************************************/


/** @file session-services.cpp
 ** Implementation of some top-level internal services of the session.
 ** The Session is _the_ central interface to access the model and thus the
 ** edit being worked on. Behind the scenes, it needs to operate several technically
 ** quite involved services, which we prefer to hide away as implementation details.
 ** Typically, each of these services defines a dedicated interface, and is implemented
 ** by delegating to a set of more specialised facilities.
 ** 
 ** The following services are integrated here
 ** - service to access a Placement by (hash) ID
 ** - service to attach or remove session content, while maintaining all indices
 ** - service to query and explore session contents
 ** - service to inject mock content for unit testing
 ** - service to manage and discover default settings by resolution query
 ** 
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#include "steam/mobject/session/session-service-fetch.hpp"
#include "steam/mobject/session/session-service-mutate.hpp"
#include "steam/mobject/session/session-service-explore-scope.hpp"
#include "steam/mobject/session/session-service-mock-index.hpp"
#include "steam/mobject/session/session-service-defaults.hpp"

#include "steam/mobject/session/session-services.hpp"
#include "steam/mobject/session/session-impl.hpp"
#include "steam/mobject/session/sess-manager-impl.hpp"

#include "steam/mobject/session/mobjectfactory.hpp"
#include "lib/symbol.hpp"

using lib::Symbol;

namespace steam {
namespace mobject {
namespace session {
  
  /** is the element-fetch service usable?
   *  Effectively this means: is the session up?
   */
  bool
  SessionServiceFetch::isAccessible ()
  {
    return Session::initFlag
        && Session::current.isUp();
  }
  
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
  
  
  /** attach an object by placement onto the session.
   *  Implemented by registering a copy of the Placement into the
   *  PlacementIndex in the session. This copy establishes a new kind of
   *  "object instance", represented by a new placement-ID, which is returned
   *  and can be used to refer to this "instance" within the session from now on.
   *  @param scope the (existing) parent scope where to attach the new element
   */
  PlacementMO::ID const&
  SessionServiceMutate::attach_toModel(PMO newPlacement, PID scope)
  {
    return SessionImplAPI::current->insertCopy (newPlacement,scope);
  }
  
  
  /** detach the denoted element from the model _including all children._
   *  @return true if actually erased something
   *  @note when specifying model root, all sub-elements will be cleared,
   *        but model root itself will be retained. 
   */
  bool
  SessionServiceMutate::detach_and_clear (PID scope)
  {
    return SessionImplAPI::current->purgeScopeRecursively (scope);
  }
  
  
  /** detach the denoted leaf element from the model.
   *  @return true if actually erased something
   *  @throw error::Fatal when attempting to remove the model root
   *  @throw error::State when the given element contains sub elements
   */
  bool
  SessionServiceMutate::detach (PID leafElement)
  {
    return SessionImplAPI::current->detachElement (leafElement);
  }

  
  
  
  namespace { // deleter function to clean up test/mock PlacementIndex
    void
    remove_testIndex (PlacementIndex* testIdx)
    {
      REQUIRE (testIdx);
      SessionImplAPI::current->reset_PlacementIndex();  // restore default Index from Session
      
      testIdx->clear();
      ASSERT (0 == testIdx->size());
      delete testIdx;
    }
  }
  
  /** Re-define the implicit PlacementIndex temporarily, e.g. for unit tests. */
  PPIdx
  SessionServiceMockIndex:: install ()
  {
    Symbol typeID ("dummyRoot");
    PMO dummyRoot (MObject::create (typeID));
    PPIdx mockIndex (new PlacementIndex(dummyRoot), &remove_testIndex); // manage instance lifecycle
    ENSURE (mockIndex);
    ENSURE (mockIndex->isValid());
    ENSURE (1 == mockIndex.use_count());
    
    SessionImplAPI::current->reset_PlacementIndex (mockIndex.get());
    return mockIndex;
  }
  
  
  /** @return resolver for DiscoveryQuery instances, actually backed by PlacementIndex */
  QueryResolver const&
  SessionServiceExploreScope::getResolver()
  {
    return SessionImplAPI::current->getScopeQueryResolver();
  }
  
  
  /** @return root scope of the current model (session datastructure) */
  PlacementMO& 
  SessionServiceExploreScope::getScope (PlacementMO const& placementToLocate)
  {
    return SessionImplAPI::current->getScope(placementToLocate);
  }


  PlacementMO& 
  SessionServiceExploreScope::getScope (PlacementMO::ID const& placementToLocate)
  {
    return SessionImplAPI::current->getScope(placementToLocate);
  }


  /** @return root scope of the current model (session datastructure) */
  PlacementMO&
  SessionServiceExploreScope::getScopeRoot()
  {
    return SessionImplAPI::current->getScopeRoot();
  }
  
  
  
}}} // namespace steam::mobject::session
