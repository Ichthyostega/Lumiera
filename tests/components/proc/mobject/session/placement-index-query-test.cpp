/*
  PlacementIndexQuery(Test)  -  querying the placement index through the generic query interface 
 
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


#include "lib/test/run.hpp"
#include "proc/mobject/session/query-resolver.hpp"
#include "proc/mobject/session/scope-query.hpp"
#include "proc/mobject/session/session-service-explore-scope.hpp"
#include "proc/mobject/session/placement-index-query-resolver.hpp"
#include "proc/mobject/session/test-scopes.hpp"
#include "lib/util.hpp"

#include <iostream>
#include <string>



namespace mobject {
namespace session {
namespace test    {
  
  using session::PathQuery;
  using session::ContentsQuery;
  using util::isSameObject;
  using std::string;
  using std::cout;
  using std::endl;
  
  
  /****************************************************************************
   * @test accessing the PlacementIndex through the generic query interface,
   *       for discovering scope contents and containing scope.
   *       
   * @todo change that to use a more realistic test session, based on the actual model types   //////////////// TICKET #532
   * 
   * @see  mobject::session::PlacementIndex
   * @see  mobject::session::QueryResolver
   * @see  mobject::session::ContentsQuery
   */
  class PlacementIndexQuery_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          checkQueryResolver();
          checkQueryOperations();
        }
      
      void
      checkQueryResolver()
        {
          PPIdx index = build_testScopes();
          QueryResolver const& resolver1 (SessionServiceExploreScope::getResolver());
          QueryResolver const& resolver2 (SessionServiceExploreScope::getResolver());
          
          ASSERT (isSameObject (resolver1, resolver2));
          
          PlacementMO& root1 = index->getRoot();          
          PlacementMO& root2 = SessionServiceExploreScope::getScopeRoot();          
          ASSERT (isSameObject (root1, root2));
          
          PlacementMO& elm1 = *ContentsQuery<TestSubMO21>(resolver1,root1);
          PlacementMO& elm2 = *(index->getReferrers(root1));
          ASSERT (isSameObject (elm1, elm2));
        }
      
      void
      checkQueryOperations()
        {
          // Prepare an (test)Index (dummy "session")
          PPIdx index = build_testScopes();
          PlacementMO& root = index->getRoot();          
          PlacementIndexQueryResolver resolver(*index);
          
          discover (ContentsQuery<MObject> (resolver,root));
          
          PlacementMO& elm = *ContentsQuery<TestSubMO1>(resolver,root);                  ////////////////////// TICKET #532
          
          discover (PathQuery<MObject> (resolver,elm));
        }
      
      
      template<class IT>
      void
      discover (IT result)
        {
          for ( ; result; ++result)
            cout << string(*result) << endl;
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementIndexQuery_test, "unit session");
  
  
}}} // namespace mobject::session::test
