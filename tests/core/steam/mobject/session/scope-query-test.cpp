/*
  ScopeQuery(Test)  -  running queries to discover container contents, filtering (sub)types 

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file scope-query-test.cpp
 ** unit test \ref ScopeQuery_test
 */


#include "lib/test/run.hpp"
#include "steam/mobject/session/scope-query.hpp"
#include "steam/mobject/session/specific-contents-query.hpp"
#include "steam/mobject/session/session-service-explore-scope.hpp"
#include "steam/mobject/session/test-scopes.hpp"
#include "steam/mobject/session/clip.hpp"
#include "lib/format-cout.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <string>



namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  using util::contains;
  using lib::Literal;
  using std::string;
  
  
  namespace { // helpers and shortcuts....
    
    /** a filter predicate to pick some objects from a resultset.
     *  @note using the specific API of DummyMO, without cast! */
    bool
    filter (Placement<DummyMO> const& candidate)
    {
      string desc = candidate->operator string();
      return contains(desc, "MO2");
    }
    
    template<class IT>
    void
    pullOut (IT const& iter)
    {
      for (IT elm(iter);
           elm; ++elm)
        cout << *elm << endl;
    }
    
    void
    announce (Literal description)
    {
      static uint nr(0);
      cout << "--------------------------------Test-"<< ++nr << ": " << description << endl;
    }
    
  }
  
  
  
  /******************************************************************************************//**
   * @test how to discover contents or location of a container-like part of the high-level model.
   *       As this container-like object is just a concept and actually implemented by the
   *       PlacementIndex, this means querying the index for elements registered with
   *       a given scope or finding the enclosing scopes. The discovered
   *       elements will be filtered by a runtime type check.
   *       
   * @todo change that to use a more realistic test session, based on the actual model types   //////////////// TICKET #532
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
          PPIdx testSession (build_testScopes());
          
          PlacementMO   const& scope = SessionServiceExploreScope::getScopeRoot();
          
          discover (ScopeQuery<MObject>    (scope, CONTENTS) , "contents depth-first");
          discover (ScopeQuery<Clip>       (scope, CONTENTS) , "contents depth-first, filtered to Clip");
          discover (ScopeQuery<DummyMO>    (scope, CONTENTS) , "contents depth-first, filtered to DummyMO");  ////////////////////// TICKET #532
          discover (ScopeQuery<TestSubMO1> (scope, CONTENTS) , "contents depth-first, filtered to TestSubMO1");
          discover (ScopeQuery<TestSubMO2> (scope, CONTENTS) , "contents depth-first, filtered to TestSubMO2");
          
          discover (pickAllSuitable(scope, filter)           , "contents depth-first, custom filtered DummyMO");
                                          // note filter is typed to accept DummyMO
          ScopeQuery<TestSubMO21> allM021(scope, CONTENTS);
          ScopeQuery<TestSubMO21>::iterator specialEl (issue(allM021));
          ++specialEl; // step in to second solution found...
          CHECK (specialEl);
          
          discover (ScopeQuery<MObject>    (*specialEl, PARENTS) , "parents of the second TestSubMO2 element found");
          discover (ScopeQuery<MObject>    (*specialEl, CHILDREN), "children of the this TestSubMO2 element");
          discover (ScopeQuery<MObject>    (*specialEl, PATH)    , "path from there to root");
          discover (ScopeQuery<TestSubMO2> (*specialEl, PATH)    , "same path, but filtered to TestSubMO2");
          announce (                                               "continue exploring partially used TestSubMO2 iterator");
          pullOut  (specialEl);
        }
      
      
      
      template<class MO>
      static void
      discover (ScopeQuery<MO> const& query, Literal description)
        {
          announce (description);
          pullOut (issue(query));
        }
      
      template<class MO>
      static typename ScopeQuery<MO>::iterator
      issue (ScopeQuery<MO> const& query)
        {
          return query.resolveBy(SessionServiceExploreScope::getResolver());
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (ScopeQuery_test, "unit session");
  
  
}}}} // namespace steam::mobject::session::test
