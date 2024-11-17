/*
  QueryFocus(Test)  -  verify proper management of current scope

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/

/** @file query-focus-test.cpp
 ** unit test \ref QueryFocus_test
 */


#include "lib/test/run.hpp"
#include "steam/mobject/session/test-scopes.hpp"
#include "steam/mobject/session/placement-index.hpp"
#include "steam/mobject/session/query-focus.hpp"
#include "steam/mobject/session/scope.hpp"
#include "lib/format-cout.hpp"



namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  namespace {
    /** Helper: extract the refcount
     *  of the current path referred by the given focus
     */
    inline size_t
    refs (QueryFocus const& focus)
    {
      return focus.currentPath().ref_count();
    }
  }
  
  
  
  
  /*******************************************************************************//**
   * @test handling of current query focus when navigating a system of nested scopes.
   *       Using a pseudo-session (actually just a PlacementIndex), this test
   *       accesses some nested scopes and then checks moving the "current scope".
   *       Moreover a (stack-like) sub-focus is created, temporarily moving aside
   *       the current focus and returning later on.
   *       
   * @see  mobject::PlacementIndex
   * @see  mobject::session::ScopePath
   * @see  mobject::session::QueryFocus
   */
  class QueryFocus_test
    : public Test
    {
      
      virtual void
      run (Arg) 
        {
          // Prepare a (test)Session with 
          // some nested dummy placements
          PPIdx index = build_testScopes();
          PMO& root = index->getRoot();
          
          QueryFocus theFocus;
          theFocus.reset();
          CHECK (Scope(root) == Scope(theFocus));
          
          checkNavigation (theFocus);
          
          Scope scopePosition = Scope(theFocus);
          manipulate_subFocus();
          
          QueryFocus currentFocus;
          CHECK (scopePosition == Scope(currentFocus));
          CHECK (currentFocus == theFocus);
          CHECK (2 == refs(currentFocus));
          CHECK (2 == refs(theFocus));
        }
      
      
      
      /** @test move the current focus to different locations
       *        and discover contents there. */
      void
      checkNavigation (QueryFocus& focus)
        {
          focus.reset();
          CHECK (Scope(focus).isRoot());
          
          PMO& someObj = *focus.query<TestSubMO1>();
                         // by construction of the test fixture,
                         // we know this object is root -> ps2 -> ps3
          
          CHECK (Scope(focus).isRoot());
          focus.shift (someObj);
          CHECK (!Scope(focus).isRoot());
          ScopePath path = focus.currentPath();
          CHECK (someObj == path.getLeaf());
          CHECK (Scope(focus).getParent().getParent().isRoot());
          
          focus.shift (path.getLeaf().getParent());
          CHECK (Scope(focus) == path.getLeaf().getParent());
          CHECK (someObj != Scope(focus));
          CHECK (path.contains (focus.currentPath()));
          CHECK (focus.currentPath().getLeaf().getParent().isRoot());
          
            // as the focus now has been moved up one level,
           //  we'll re-discover the original starting point as immediate child
          CHECK (someObj == *focus.explore<TestSubMO1>());
        }
      
      
      
      /** @test side-effect free manipulation of a sub-focus,
       *        while the original focus is pushed aside (stack) */
      void
      manipulate_subFocus()
        {
          QueryFocus original;    // automatically attaches to current stack top
          uint num_refs = refs (original);
          CHECK (num_refs > 1);   // because the run() function also holds a ref
          
          QueryFocus subF = QueryFocus::push();
          cout << subF << endl;
          CHECK (subF == original);
          
          CHECK (       1 == refs(subF) );
          CHECK (num_refs == refs(original));
          
          { // temporarily creating an independent focus attached differently
            QueryFocus subF2 = QueryFocus::push(Scope(subF).getParent());
            CHECK (subF2 != subF);
            CHECK (subF == original);
            cout << subF2 << endl;
            
            ScopeQuery<TestSubMO21>::iterator ii = subF2.explore<TestSubMO21>();
            while (ii) // drill down depth first
              {
                subF2.shift(*ii);
                cout << subF2 << endl;
                ii = subF2.explore<TestSubMO21>();
              }
            cout << subF2 << "<<<--discovery exhausted" << endl;
            
            subF2.pop(); // releasing this focus and re-attaching to what's on stack top
            cout << subF2 << "<<<--after pop()" << endl;
            CHECK (subF2 == subF);
            CHECK (2 == refs(subF2));   // both are now attached to the same path
            CHECK (2 == refs(subF));
          }
          // subF2 went out of scope, but no auto-pop happens (because subF is still there)
          cout << subF << endl;
          
          CHECK (       1 == refs(subF));
          CHECK (num_refs == refs(original));
         // when subF goes out of scope now, auto-pop will happen...
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (QueryFocus_test, "unit session");
  
  
}}}} // namespace steam::mobject::session::test
