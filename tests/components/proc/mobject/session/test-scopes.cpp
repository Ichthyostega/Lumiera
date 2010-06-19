/*
  TEST-SCOPES.cpp  -  builds a test PlacementIndex containing dummy Placements as nested scopes
 
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
 
* *****************************************************/


#include "proc/mobject/session/test-scopes.hpp"
#include "proc/mobject/session/session-service-mock-index.hpp"
#include "proc/mobject/session/session-service-explore-scope.hpp"
#include "proc/mobject/session/scope-query.hpp"


namespace mobject {
namespace session {
namespace test    {
  
  
  /** @note this dummy index isn't actively connected to the session;
   *        the unit tests rely on this dummy index containing
   *        a specific tree structure of test-dummy MObjects. 
   */
  PPIdx
  build_testScopes()
  {
    PDum p1(*new TestSubMO21);
    PDum p2(*new TestSubMO21);
    PDum p3(*new TestSubMO21);
    PDum p4(*new TestSubMO21);
    PDum p5(*new TestSubMO21);
    
    PDum ps1(*new DummyMO);
    PDum ps2(*new TestSubMO2);
    PDum ps3(*new TestSubMO1);
    
    // Prepare an (test)Index backing the PlacementRefs
    PPIdx index (SessionServiceMockIndex::install());
    
    PMO& root = index->getRoot();

    typedef PMO::ID ID;
    ID i1 = index->insert (p1, root);
    ID i2 = index->insert (p2,  i1 );
    ID i3 = index->insert (p3,  i2 );
    ID i4 = index->insert (p4,  i3 );
    ID i5 = index->insert (p5,  i4 );
    
    ID is1 = index->insert (ps1,root);
    ID is2 = index->insert (ps2,root);
    ID is3 = index->insert (ps3, is2);
    
    return index;
  }
  
  
  /** @note this test helper only works if build_testScopes is invoked
   *  beforehand, and the returned smart-ptr to the created test/dummy index
   *  is retained. Moreover, this function makes assumptions about the actual
   *  objects created by the former test function.
   *  @throw lumiera::error::Invalid if the intended start element doesn't exist (anymore)
   */
  PlacementMO&
  retrieve_startElm()
  {
    return *ContentsQuery<TestSubMO1>(SessionServiceExploreScope::getScopeRoot())
                           .resolveBy(SessionServiceExploreScope::getResolver());
  }
  
  
  ScopeQuery<MObject>::iterator
  explore_testScope (PlacementMO const& scopeTop)
  {
    return ScopeQuery<MObject>(scopeTop, CHILDREN)
                           .resolveBy(SessionServiceExploreScope::getResolver());
  }
  
  
}}} // namespace mobject::session::test
