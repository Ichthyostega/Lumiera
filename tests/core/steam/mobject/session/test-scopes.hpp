/*
  TEST-SCOPES.hpp  -  builds a test PlacementIndex containing dummy Placements as nested scopes

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file test-scopes.hpp
 ** Unit test helper to generate a system of nested test scopes.
 ** These can be used to verify session search and discovery functionality.
 ** The build_testScopes() function generates a system of placements and MObjects
 ** with a known, fixed layout.
 */


#ifndef MOBJECT_SESSION_TEST_TEST_SCOPES_H
#define MOBJECT_SESSION_TEST_TEST_SCOPES_H


#include "steam/mobject/test-dummy-mobject.hpp"
#include "steam/mobject/session/placement-index.hpp"
#include "steam/mobject/session/scope-query.hpp"
#include "steam/mobject/placement.hpp"

#include <memory>


namespace proc {
namespace mobject {
namespace session {
namespace test    {
  
  using namespace mobject::test;
  typedef TestPlacement<DummyMO> PDum;
  
  typedef std::shared_ptr<PlacementIndex> PPIdx;
  typedef ScopeQuery<MObject>::iterator _ScopeIterMO;
  
  
  
  /** helper for tests: create a pseudo-session (actually just a PlacementIndex),
   *  which contains some nested placement scopes.
   *  @return new PlacementIndex, which has already been activated to be used
   *          by all Placements from now on. This activation will be cleared
   *          automatically, when this object goes out of scope.
   * 
   * @see mobject::PlacementIndex
   * @see session::SessManagerImpl::getCurrentIndex()
   * @see mobject::session::SessionServiceMockIndex::reset_PlacementIndex
   */
  PPIdx build_testScopes();
  
  
  /** complement to the helper: retrieve one of the dummy placements
   *  which is a Placement<> and way down into the hierarchy
   */
  PlacementMO& retrieve_startElm();

  PlacementMO& retrieve_firstTestSubMO21();
  
  
  /** shortcut to explore the contents of a scope within the current index.
   *  Usually, clients would use QueryFocus or ScopeLocator to perform this task,
   *  but for the purpose of testing we're better off to invoke the query directly
   */
  _ScopeIterMO explore_testScope (PlacementMO const& scopeTop);
  
  
  
}}}} // namespace proc::mobject::session::test
#endif
