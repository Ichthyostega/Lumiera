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
   * @see  mobject::session::PlacementIndex
   * @see  mobject::session::QueryResolver
   * @see  mobject::session::ContentsQuery
   */
  class PlacementIndexQuery_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          checkQueryResolverWrapper();
          checkQueryOperations();
        }
      
      void
      checkQueryResolverWrapper()
        {
          PPIdx index = build_testScopes();
          QueryResolver const& resolver1 (*index);
          QueryResolver const& resolver2 (*index);
          
          ASSERT (isSameObject (resolver1, resolver2));
          index = build_testScopes();
        }
      
      void
      checkQueryOperations()
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #384  !!!!!!!!!
          // Prepare an (test)Index (dummy "session")
          PPIdx index = build_testScopes();
          PlacementIndexQueryResolver resolver(index);
          
          discover (ContentsQuery<MObject> (resolver));
          
          PlacementMO& elm = *ContentsQuery<TestSubMO21>(resolver);
          
          discover (PathQuery(resolver,elm));
#endif ////////////////////////////////////////////////////////////////////////////////////////TODO lots of things unimplemented.....!!!!!
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
