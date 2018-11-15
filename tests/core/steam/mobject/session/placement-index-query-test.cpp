/*
  PlacementIndexQuery(Test)  -  querying the placement index through the generic query interface 

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

* *****************************************************/

/** @file placement-index-query-test.cpp
 ** unit test \ref PlacementIndexQuery_test
 */


#include "lib/test/run.hpp"
#include "steam/mobject/session/scope-query.hpp"
#include "steam/mobject/session/session-service-explore-scope.hpp"
#include "steam/mobject/session/placement-index-query-resolver.hpp"
#include "steam/mobject/session/test-scopes.hpp"
#include "common/query/query-resolver.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"



namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  using lumiera::QueryResolver;
  using session::PathQuery;
  using session::ContentsQuery;
  using util::isSameObject;
  
  
  /************************************************************************//**
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
          
          CHECK (isSameObject (resolver1, resolver2));
          
          PlacementMO& root1 = index->getRoot();          
          PlacementMO& root2 = SessionServiceExploreScope::getScopeRoot();          
          CHECK (isSameObject (root1, root2));
          
          PlacementMO& elm1 = *ContentsQuery<TestSubMO2>(root1).resolveBy(resolver1);
          PlacementMO& elm2 = *ContentsQuery<TestSubMO1>(root1).resolveBy(resolver1);
          PlacementMO& elm3 = *(index->getReferrers(elm1));
          CHECK (isSameObject (elm3, elm2));
          // relying on the specific setup of the test index
          // MO1 is the sole "referrer" of MO2 (the only content within MO2's scope)
          // root \ TestSubMO2 \ TestSubMO1
        }
      
      void
      checkQueryOperations()
        {
          // Prepare an (test)Index (dummy "session")
          PPIdx index = build_testScopes();
          PlacementMO& root = index->getRoot();          
          PlacementIndexQueryResolver resolver(*index);
          
          cout << "explore contents depth-first..." << endl;
          discover (ContentsQuery<MObject>(root).resolveBy(resolver));
          
          PlacementMO& elm = *ContentsQuery<TestSubMO1>(root).resolveBy(resolver);                  ////////////////////// TICKET #532
          
          cout << "path to root starting at " << elm << endl;
          discover (PathQuery<MObject> (elm).resolveBy(resolver));
        }
      
      
      template<class IT>
      void
      discover (IT result)
        {
          for ( ; result; ++result)
            cout << *result << endl;
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementIndexQuery_test, "unit session");
  
  
}}}} // namespace proc::mobject::session::test
