/*
  QueryFocus(Test)  -  verify proper management of current scope
 
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
#include "proc/mobject/session/test-scopes.hpp"
#include "proc/mobject/placement-index.hpp"
#include "proc/mobject/session/query-focus.hpp"
#include "proc/mobject/session/scope.hpp"
//#include "lib/util.hpp"

#include <iostream>
#include <string>



namespace mobject {
namespace session {
namespace test    {
  
  //using util::isSameObject;
  //using lumiera::Time;
  using std::string;
  using std::cout;
  using std::endl;
  
  
  /**********************************************************************************
   * @test handling of current query focus when navigating a system of nested scopes.
   *       Using a pseudo-session (actually just a PlacementIndex), this test
   *       creates some nested scopes and then checks moving the "current scope".
   *       
   * @see  mobject::PlacementIndex
   * @see  mobject::session::ScopePath
   * @see  mobject::session::QueryFocus
   */
  class QueryFocus_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          
          // Prepare an (test)Index backing the PlacementRefs
          PPIdx index = build_testScopes();
          PMO& root = index->getRoot();
          
          UNIMPLEMENTED ("unit test to cover query focus management");
          
          QueryFocus theFocus;
          theFocus.reset();
          ASSERT (Scope(root) == Scope(theFocus));
          
          checkNavigation (theFocus);
          
          Scope scopePosition = Scope(theFocus);
          manipulate_subFocus();
          
          QueryFocus currentFocus;
          ASSERT (scopePosition == Scope(currentFocus));
          ASSERT (currentFocus == theFocus);
        }
      
      
      /** @test move the current focus to various locations
       *        and discover contents there. */
      void
      checkNavigation (QueryFocus& focus)
        {
          focus.reset();
          ASSERT (Scope(focus).isRoot());
          
          PMO& someObj = focus.query<TestSubMO1>();
                         // by construction of the test fixture,
                         // we know this object is root -> ps2 -> ps3
          
          ASSERT (Scope(focus).isRoot());
          focus.attach (someObj);
          ASSERT (!Scope(focus).isRoot());
          ScopePath path = focus.currentPath();
          ASSERT (someObj == path.getLeaf());
          ASSERT (path.getParent().getParent().isRoot());
          
          focus.attach (path.getParent());
          ASSERT (Scope(focus) == path.getParent());
          ASSERT (someObj != Scope(focus));
          ASSERT (path.contains (focus.currentPath()));
          ASSERT (focus.currentPath().getParent().isRoot());
        }
      
      
      /** @test side-effect free manipulation of a sub-focus */
      void
      manipulate_subFocus()
        {
          QueryFocus original;
          uint num_refs = original.ref_count();
          ASSERT (num_refs > 1);
          
          QueryFocus subF = QueryFocus::push();
          cout << string(subF) << endl;
          ASSERT (subF == original);
          
          ASSERT (       1 == subF.ref_count());
          ASSERT (num_refs == original.ref_count());
          
          {
            QueryFocus subF2 = QueryFocus::push(Scope(subF).getParent());
            ASSERT (subF2 != subF);
            ASSERT (subF == original);
            cout << string(subF2) << endl;
            
            Iterator ii = subF2.query<TestSubMO21>();
            while (ii)
              {
                subF2.attach(*ii);
                cout << string(subF2) << endl;
                ii = subF2.query<TestSubMO21>();
              }
            cout << string(subF2) << "<<<--discovery exhausted" << endl;
            
            subF2.pop();
            cout << string(subF2) << "<<<--after pop()" << endl;
            ASSERT (subF2 == subF);
            ASSERT (2 == subF2.ref_count());
            ASSERT (2 == subF.ref_count());
          }
          // subF2 went out of scope, but no auto-pop happens
          cout << string(subF) << endl;
          
          ASSERT (       1 == subF.ref_count());
          ASSERT (num_refs == original.ref_count());
         // when subF goes out of scope now, auto-pop will happen...
        }
          
    };
  
  
  /** Register this test class... */
  LAUNCHER (QueryFocus_test, "unit session");
  
  
}}} // namespace mobject::session::test
