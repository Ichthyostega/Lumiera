/*
  ScopeQuery(Test)  -  running queries to discover container contents, filtering (sub)types 
 
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
#include "proc/mobject/session/session-service-explore-scope.hpp"
#include "proc/mobject/session/scope-query.hpp"
#include "proc/mobject/session/test-scopes.hpp"
//#include "lib/util.hpp"

#include <iostream>
#include <string>



namespace mobject {
namespace session {
namespace test    {
  
  using std::string;
  using std::cout;
  using std::endl;
  
  
  /**********************************************************************************************
   * @test how to discover contents or location of a container-like part of the high-level model.
   *       As this container-like object is just a concept and actually implemented by the
   *       PlacementIndex, this means querying the index for elements registered with
   *       a given scope or finding the enclosing scopes. The discovered
   *       elements will be filtered by a runtime type check.
   *       
   * @todo cover using an additional dynamic filter on the results
   *       
   * @see  mobject::session::PlacementIndex
   * @see  mobject::session::QueryResolver
   * @see  mobject::session::ContentsQuery
   */
  class ScopeQuery_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          // Prepare an (test)Index (dummy "session")
          build_testScopes();
          
          QueryResolver const& resolver = SessionServiceExploreScope::getResolver();
          PlacementMO   const& scope    = SessionServiceExploreScope::getScopeRoot();
          
          discover (ScopeQuery<MObject>    (resolver,scope, CONTENTS));
          discover (ScopeQuery<DummyMO>    (resolver,scope, CONTENTS));
          discover (ScopeQuery<TestSubMO1> (resolver,scope, CONTENTS));
          discover (ScopeQuery<TestSubMO2> (resolver,scope, CONTENTS));
          
          ScopeQuery<TestSubMO21> specialEl(resolver,scope, CONTENTS);
          
          discover (ScopeQuery<MObject>    (resolver,*specialEl, PARENTS));
          discover (ScopeQuery<MObject>    (resolver,*specialEl, PATH));
          discover (ScopeQuery<TestSubMO2> (resolver,*specialEl, PATH));
          discover (specialEl);
        }
      
      
      template<class MO>
      void
      discover (ScopeQuery<MO> const& query)
        {
          typedef typename ScopeQuery<MO>::iterator I;
          
          for (I elm(query);
               elm; ++elm)
            cout << string(*elm) << endl;
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (ScopeQuery_test, "unit session");
  
  
}}} // namespace mobject::session::test
