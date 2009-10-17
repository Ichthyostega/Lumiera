/*
  TEST-SCOPES.hpp  -  builds a test PlacementIndex containing dummy Placements as nested scopes
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef MOBJECT_SESSION_TEST_TEST_SCOPES_H
#define MOBJECT_SESSION_TEST_TEST_SCOPES_H


//#include "lib/lumitime.hpp"
//#include "proc/mobject/placement-ref.hpp"
#include "proc/mobject/session/test-scopes.hpp"
#include "proc/mobject/test-dummy-mobject.hpp"
#include "proc/mobject/placement-index.hpp"
//#include "lib/util.hpp"

#include <tr1/memory>
//#include <iostream>
//#include <string>

//using util::isSameObject;
//using lumiera::Time;
//using std::string;
//using std::cout;
//using std::endl;


namespace mobject {
namespace session {
namespace test    {
  
  using std::tr1::shared_ptr;

  using namespace mobject::test;
  typedef TestPlacement<DummyMO> PDum;
  
  
  
  /** helper for tests: create a pseudo-session (actually just a PlacementIndex),
   *  which contains some nested placement scopes.
   *  @return new PlacementIndex, which has already been activated to be used
   *          by all Placements from now on. This activation will be cleared
   *          automatically, when this object goes out of scope.
   * 
   * @see mobject::PlacementIndex
   * @see session::SessManagerImpl::getCurrentIndex()
   * @see mobject::reset_PlacementIndex
   */
  PPIdx build_testScopes();
  
  
}}} // namespace mobject::session::test
#endif
