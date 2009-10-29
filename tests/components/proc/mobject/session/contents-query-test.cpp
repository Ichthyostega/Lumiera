/*
  ContentsQuery(Test)  -  running queries to discover container contents, filtering (sub)types 
 
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
//#include "lib/lumitime.hpp"
//#include "proc/mobject/placement-ref.hpp"
#include "proc/mobject/session/query-resolver.hpp"
#include "proc/mobject/session/contents-query.hpp"
#include "proc/mobject/session/test-scopes.hpp"
//#include "proc/mobject/placement-index.hpp"
//#include "lib/util.hpp"

#include <iostream>
//#include <string>



namespace mobject {
namespace session {
namespace test    {

  using session::ContentsQuery;
  using session::Query;
  //using util::isSameObject;
  //using lumiera::Time;
  //using std::string;
  using std::cout;
  using std::endl;

  
  /***************************************************************************************
   * @test how to discover the contents of a container-like part of the high-level model.
   *       As this container-like object is just a concept and actually implemented
   *       by the PlacementIndex, this includes enumerating a scope. The discovered
   *       contents may be additionally filtered by a runtime type check.
   *       
   * @see  mobject::PlacementIndex
   * @see  mobject::session::QueryResolver
   * @see  mobject::session::ContentsQuery
   */
  class ContentsQuery_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          
          // Prepare an (test)Index backing the PlacementRefs
          PPIdx index = build_testScopes();
          PlacementMO scope (index->getRoot());
          
          discover (ContentsQuery<MObject>    (index,scope));
          discover (ContentsQuery<DummyMO>    (index,scope));
          discover (ContentsQuery<TestSubMO1> (index,scope));
          discover (ContentsQuery<TestSubMO2> (index,scope));
          discover (ContentsQuery<TestSubMO21>(index,scope));
        }
      
      
      template<class MO>
      void
      discover (Query<MO> const& query)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
          Query<MO>::iterator elm = query();
          while (elm)
            cout << *elm++ << endl;
#endif ////////////////////////////////////////////////////////////////////////////////////////TODO lots of things unimplemented.....!!!!!
        }
          
    };
  
  
  /** Register this test class... */
  LAUNCHER (ContentsQuery_test, "unit session");
  
  
}}} // namespace mobject::session::test
