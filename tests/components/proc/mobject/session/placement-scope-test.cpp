/*
  PlacementScope(Test)  -  accessing and navigating placement scope
 
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
#include "lib/test/test-helper.hpp"
#include "proc/mobject/session/scope.hpp"
#include "proc/mobject/session/test-scopes.hpp"
//#include "lib/lumitime.hpp"
//#include "proc/mobject/placement-ref.hpp"
//#include "proc/mobject/session/placement-index.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
#include "lib/util.hpp"

#include <iostream>
//#include <string>



namespace mobject {
namespace session {
namespace test    {
  
//  using namespace mobject::test;
  using lumiera::error::LUMIERA_ERROR_INVALID;
  
  using util::isSameObject;
  //using lumiera::Time;
  //using std::string;
  using std::cout;
  using std::endl;
  
  
  
  /***************************************************************************
   * @test basic behaviour of the nested placement search scopes.
   *       Using a pseudo-session (actually just a PlacementIndex),
   *       this test creates some nested scopes and then...
   *       - discovers the scope of a placement
   *       - finds the parent scope
   *       - enumerates a scope path up to root
   * 
   * @see  mobject::Placement
   * @see  mobject::session::ScopePath
   * @see  mobject::session::QueryFocus
   */
  class PlacementScope_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          // Prepare an (test)Index backing the PlacementRefs
          PPIdx index = build_testScopes();
          
          verifyEquality();
          UNIMPLEMENTED ("function test of placement scope interface");
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
          verifyLookup (index);
          verifyNavigation (index);
        }
      
      
      typedef Query<Placement<DummyMO> >::iterator _Iter;
      
      _Iter
      getContents(PPIdx index)
        {
          return index->query<DummyMO>(index->getRoot());
#endif     //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
        }
      
      /** @test for each Placement in our test "session",
       *        find the scope and verify it's in line with the index
       */
      void
      verifyLookup (PPIdx ref_index)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
          for (_Iter elm = getContents(ref_index); elm; ++elm)
            {
              ASSERT (elm->isValid());
              cout << string(*elm) << endl;
              Scope const& scope1 = Scope::containing(*elm);
              
              RefPlacement ref (*elm);
              Scope const& scope2 = Scope::containing(ref);
              
              // verify this with the scope registered within the index...
              PlacementMO& scopeTop = ref_index->getScope(*elm);
              ASSERT (scope1 == scopeTop);
              ASSERT (scope2 == scopeTop);
              ASSERT (scope1 == scope2);
              
              ASSERT (isSameObject (scope1,scope2));
            }
#endif     //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
        }
      
      
      /** @test equality of scopes is based on the ID of the scope top (Placement) */
      void
      verifyEquality ()
        {
          PlacementMO& aPlac = retrieve_startElm();
          Scope scope1(aPlac);
          Scope scope2(aPlac);
          Scope nil;
          
          ASSERT (scope1 == scope2); ASSERT (scope2 == scope1);
          ASSERT (scope1 != nil);    ASSERT (nil != scope1);
          ASSERT (scope2 != nil);    ASSERT (nil != scope2);
          
          ASSERT (aPlac == scope1);  ASSERT (scope1 == aPlac);  
          ASSERT (aPlac == scope2);  ASSERT (scope2 == aPlac);  
          ASSERT (aPlac != nil);     ASSERT (nil != aPlac);
          
          Scope par (scope1.getParent());
          ASSERT (scope1 != par);    ASSERT (par != scope1);
          ASSERT (scope2 != par);    ASSERT (par != scope2);
          
          PlacementMO& placm2 (scope2.getTop());
          ASSERT (aPlac.getID() == placm2.getID());
          
          PlacementMO& parPlac (par.getTop());
          ASSERT (aPlac.getID() != parPlac.getID());
        }
      
      
      /** @test navigate to root, starting from each Placement */
      void
      verifyNavigation (PPIdx ref_index)
        {
#if false  //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
          for (_Iter elm = getContents(ref_index); elm; ++elm)
            {
              Scope const& scope = Scope::containing(*elm);
              ASSERT (scope == *scope.ascend());
              for (Scope::iterator sco = scope.ascend(); sco; ++sco)
                if (sco->isRoot())
                  {
                    VERIFY_ERROR (INVALID, sco->getParent() );
                    RefPlacement top = sco->getTop();
                    RefPlacement root = ref_index->getRoot();
                    
                    ASSERT (isSameObject (top,root));
                  }
                else
                  {
                    Scope& parent = sco->getParent();
                    RefPlacement top = sco->getTop();
                    Scope& parentsScope = Scope::containing(top);
                    RefPlacement topsTop = ref_index->getScope(top); ///////////////////TODO impact of Binding a Sequence? see Ticket #311
                    ASSERT (topsTop == parentsScope);
                    ASSERT (isSameObject (topsTop, parentsScope.getTop()));
            }     }
#endif     //////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET 384  !!!!!!!!!
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementScope_test, "function session");
  
  
}}} // namespace mobject::session::test
