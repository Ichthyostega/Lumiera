/*
  TEST-SCOPES.cpp  -  builds a test PlacementIndex containing dummy Placements as nested scopes

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file test-scopes.cpp
 ** Implementation of nested test scopes to cover the placement index.
 ** This translation unit builds mock "session content" with a fixed, known layout,
 ** which can be used to verify the behaviour of session query and content discovery
 */


#include "steam/mobject/session/test-scopes.hpp"
#include "steam/mobject/session/session-service-mock-index.hpp"
#include "steam/mobject/session/session-service-explore-scope.hpp"
#include "steam/mobject/session/scope-query.hpp"


namespace steam    {
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
            index->insert (p5,  i4 );
    
            index->insert (ps1,root);
    ID is2 =index->insert (ps2,root);
            index->insert (ps3, is2);
    
    return index;
  }
  
  namespace {
    template<class MO>
    PlacementMO&
    retrieve_firstElmOfType()
    {
      return *ContentsQuery<MO>(SessionServiceExploreScope::getScopeRoot())
                     .resolveBy(SessionServiceExploreScope::getResolver());
    }
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
    return retrieve_firstElmOfType<TestSubMO1>();
  }
  
  PlacementMO&
  retrieve_firstTestSubMO21()
  {
    return retrieve_firstElmOfType<TestSubMO21>();
  }
  
  
  ScopeQuery<MObject>::iterator
  explore_testScope (PlacementMO const& scopeTop)
  {
    return ScopeQuery<MObject>(scopeTop, CHILDREN)
                           .resolveBy(SessionServiceExploreScope::getResolver());
  }
  
  
}}}} // namespace steam::mobject::session::test
